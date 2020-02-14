#ifndef PARSERSMANAGER_H
#error "Do not include this file directly! Use parsersmanager.h!"
#endif

#include <netinet/in.h>
#include <QObject>
#include "parsers/jsonparser.h"
#include "parsers/structparser.h"
#include "widget.h"
#include "somestruct.h"
#include "structs/kdfrom_ksa4.h"
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
    auto pars1 = std::make_shared<StructParserLight<kd_97L6_01a>>(0x0001);
    _dataParsers.insert({0x0001, pars1});
    auto pars2 = std::make_shared<StructParserLight<DataOne>>(0x0101);
    _dataParsers.insert({0x0101, pars2});
    auto pars3 = std::make_shared<StructParserLight<DataTwo>>(0x0202);
    _dataParsers.insert({0x0202, pars3});
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
void ParsersManager<S, PFamily>::parseMsg(char* dataFromTcp, int size)
{
    std::string data(dataFromTcp, static_cast<size_t>(size));
    // Добавляем кусок сообщения, оставшийся после разбора предыдущего сообщения:
    data.insert(0, _pieceOfData);
    _pieceOfData.clear();
    if (data.size() <= _header->emptyMsgLen())
    {   // Отбрасываем пустые и слишком короткие сообщения:
        qDebug() << "Bad message content or empty message. Skip it.";
        return;
    }

//    std::string test = testData();
//    if (test.size() != 0)
//        data = std::move(test);
    readMsgFromBeginning(std::move(data)); // Решение подзадачи "Чтение сообщения"
}

template<>
std::string ParsersManager<EmptyHeader, AbstractP>::testData()
{
    std::string str;
    str.resize(300);
    uint pos = 0;

    ushort type = 0x0101;
    memcpy(&str[pos], &type, sizeof(type));
    uint len = 8;
    len = htonl(len);
    memcpy(&str[pos + 2], &len, sizeof(len));

    DataOne dOne{'e', 1000};
    memcpy(&str[pos + 6], &dOne, sizeof(dOne));
    str.resize(6 + sizeof(dOne));

    return str;
}


template<>
void ParsersManager<EmptyHeader, AbstractP>::readMsgFromBeginning(std::string &&data, EmptyHeader* /*ptr*/)
{
    for (auto &parser : _dataParsers)
        parser.second->clearCollection();

    static int number = 0;
    while (data.size() >= 6)
    {   // Выбираем парсер:
        auto type = convert(data.substr(0, 2));
//        qDebug() << ++number << ". Type of struct:" << type;
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
    number = 0;

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
    {
        obj.second->useData();
//        decltype (obj<>) a;
        // Обхожу все контейнеры, разбираю все сообщения.
    }

    // Решение - шаблонная функция (скорее всего, другого класса),
    // специализации которой охватят все типы и смогут работать со всеми
    // контейнерами.
}

