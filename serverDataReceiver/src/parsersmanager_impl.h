#ifndef PARSERSMANAGER_H
#error "Do not include this file directly! Use parsersmanager.h!"
#endif

#include <netinet/in.h>
#include <QObject>
#include "parsers/jsonparser.h"
#include "parsers/structparser.h"
#include "widget.h"
#include "structs/somestruct.h"
#include "structs/struct001.h"
#include "all_struct_parser/structparserlight.h"


template<typename S, typename PFamily>
ParsersManager<S, PFamily>::ParsersManager(std::weak_ptr<Widget> w, S header)
    : _widget(w)
    , _header(std::make_shared<HeaderDescription<S>>(header))
{
}

template<>
void ParsersManager<DataHeader, AbstractParser>::init()
{
    //    _jsonParser   = std::make_shared<JsonParser<DataHeader>>(this);
    //    _structParser = std::make_shared<StructParser<SomeStruct, DataHeader>>(this);
        auto jsonParser   = std::make_shared<JsonParser>
                (std::enable_shared_from_this
                 <ParsersManager<DataHeader, AbstractParser>>::shared_from_this() );
        auto structParser = std::make_shared<StructParser<SomeStruct>>
                (std::enable_shared_from_this
                 <ParsersManager<DataHeader, AbstractParser>>::shared_from_this() );
        // 2 байта, определяющие тип передаваемых данных:
        _dataParsers.insert({convert("JJ"), jsonParser});
        _dataParsers.insert({convert("SS"), structParser});
        // Связываем сигналы объектов производных классов со слотами:
        QObject::connect(jsonParser.get(), SIGNAL(messageParsingComplete(MessageParsingResult)),
                         _widget.lock().get(),  SLOT(printParsingResults(MessageParsingResult)) );
        QObject::connect(structParser.get(), SIGNAL(messageParsingComplete(MessageParsingResult)),
                         _widget.lock().get(),  SLOT(printParsingResults(MessageParsingResult)) );
}

template<>
void ParsersManager<EmptyHeader, AbstractP>::init()
{
    auto pars1 = std::make_shared<StructParserLight<Struct001>>(0x0001);
    _dataParsers.insert({pars1->getNum(), pars1});
    auto pars2 = std::make_shared<StructParserLight<DataOne>>(0x0101);
    _dataParsers.insert({pars2->getNum(), pars2});
    auto pars3 = std::make_shared<StructParserLight<DataTwo>>(0x0202);
    _dataParsers.insert({pars3->getNum(), pars3});
}

template<typename S, typename PFamily>
std::shared_ptr<ParsersManager<S, PFamily>>
ParsersManager<S, PFamily>::create(std::weak_ptr<Widget> w, /*std::shared_ptr<S>*/ S header)
{
    auto ptr = std::shared_ptr<ParsersManager<S, PFamily>>
            (new ParsersManager<S, PFamily>(w, header));
    ptr->init();
    return ptr;
}

template<typename S, typename PFamily>
ParsersManager<S, PFamily>::~ParsersManager()
{
    qDebug() << "ParsersManager::~dtor() called";
}

template<typename S, typename PFamily>
std::shared_ptr<HeaderDescription<S>>
ParsersManager<S, PFamily>::headerDescription() const
{
    return _header;
}

template<typename S, typename PFamily>
void ParsersManager<S, PFamily>::savePieceOfData(std::string&& piece)
{
    _pieceOfData = std::move(piece);
}

template<typename S, typename PFamily>
typename ParsersManager<S, PFamily>::ShPtrPFamily
ParsersManager<S, PFamily>::chooseParserByDataType(ushort type)
{
    auto it = _dataParsers.find(type);
    if (it == _dataParsers.end())
        return nullptr;
    else {
        return (*it).second;
    }
}

template<typename S, typename PFamily>
void ParsersManager<S, PFamily>::parseMsg(std::vector<char> &&data)
{
    std::string dataStr(data.data(), data.size());
    // Добавляем кусок сообщения, оставшийся после разбора предыдущего сообщения:
    dataStr.insert(0, _pieceOfData);
    _pieceOfData.clear();
    if (dataStr.size() <= _header->emptyMsgLen())
    {   // Отбрасываем пустые и слишком короткие сообщения:
        qDebug() << "Bad message content or empty message. Skip it.";
        return;
    }

    // Temp: временный отладочный код, для проверки разбора struct DataOne:
    std::string td = testData();
    if (td.size() != 0)
        dataStr = std::move(td);
    readMsgFromBeginning(std::move(dataStr)); // Решение подзадачи "Чтение сообщения"
}


template<>
const std::map<ushort, std::shared_ptr<AbstractP>>*
ParsersManager<EmptyHeader, AbstractP>::getMapOfParsers() const
{
    return &_dataParsers;
}

template<>
const std::map<ushort, std::shared_ptr<AbstractParser>>*
ParsersManager<DataHeader, AbstractParser>::getMapOfParsers() const
{
    return nullptr;
}


template<>
std::string ParsersManager<EmptyHeader, AbstractP>::testData()
{
    std::string str;
    str.resize(300);
    uint pos = 0;

    ushort type = 0x0101;
    memcpy(&str[pos], &type, sizeof(type));
    uint len = sizeof(DataOne);
    len = htonl(len);
    memcpy(&str[pos + 2], &len, sizeof(len));

    DataOne dOne{'e', 1000};
    memcpy(&str[pos + 6], &dOne, sizeof(dOne));
    str.resize(6 + sizeof(dOne));

    return str;
}


template<>
void ParsersManager<EmptyHeader, AbstractP>::readMsgFromBeginning(std::string &&data, EmptyHeader* /*ptr*/)
{   // Очищаем контейнеры, содержащие результаты прошлого разбора:
    for (auto &parser : _dataParsers)
        parser.second->clearCollection();

    while (data.size() >= 6)
    {   // Выбираем парсер:
        auto type = convert(data.substr(0, 2));
        _currentParser = chooseParserByDataType(type);
        if (_currentParser == nullptr)
        {
            qDebug() << QObject::tr("Ошибка в заголовке сообщения: невозможно определить тип данных");
            return;
        }
        // Извлекаем длину сообщения:
        uint len;
        memcpy(&len, &data[2], sizeof(len));
        len = htonl(len);
//        qDebug() << "Data len = " << len;
        // Если часть рассматриваемого блока данных находится за пределами
        // текущего сообщения:
        if (len + 6 > data.size())
        {
//                qDebug() << tr("Вторая часть рассматриваемого блока данных "
//                               "находится за пределами текущего сообщения.");
            savePieceOfData(std::move(data));
            return;
        }
        else {
            data.erase(0, 6);
            _currentParser->initStruct(data, len);
        }
    }
    if (data.size() > 0)
    {
        qDebug() << QObject::tr("Количество оставшихся байт < 6. Сохраняем их.");
        savePieceOfData(std::move(data));
    }
}

template<typename S, typename PFamily>
void ParsersManager<S, PFamily>::printSturctsContent()
{
    for (auto obj : _dataParsers)
        obj.second->useData();
}

