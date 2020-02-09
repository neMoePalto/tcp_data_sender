#ifndef PARSERSMANAGER_H
#error "Do not include this file directly! Use parsersmanager.h!"
#endif

#include <netinet/in.h>
#include <QObject>
#include "parsers/jsonparser.h"
#include "parsers/structparser.h"
#include "widget.h"
#include "somestruct.h"
//#include <QDebug>
//#include "headerdescription.h"
#include "all_struct_parser/unistructparser.h"

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
        _dataParsers.insert({"JJ", jsonParser});
        _dataParsers.insert({"SS", structParser});
        // Связываем сигналы объектов производных классов со слотами:
        QObject::connect(jsonParser.get(), SIGNAL(messageParsingComplete(MessageParsingResult)),
                         _widget.lock().get(),  SLOT(printParsingResults(MessageParsingResult)) );
        QObject::connect(structParser.get(), SIGNAL(messageParsingComplete(MessageParsingResult)),
                         _widget.lock().get(),  SLOT(printParsingResults(MessageParsingResult)) );


        // Experimental:
        auto parser_01 = std::make_shared<UniStructParser<SomeStruct>>();
        _dataParsers_2.insert({0x01AA, parser_01});

}

template<>
void ParsersManager<EmptyHeader, AbstractP>::init()
{}

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
std::shared_ptr<HeaderDescription<S>> ParsersManager<S, PFamily>::headerDescription() const
{
    return _header;
}

template<typename S, typename PFamily>
void ParsersManager<S, PFamily>::savePieceOfData(std::string&& piece)
{
    _pieceOfData = std::move(piece);
}

template<typename S, typename PFamily>
typename ParsersManager<S, PFamily>::ShPtrAbstractParser
ParsersManager<S, PFamily>::chooseParserByDataType(const std::string& type)
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
    readMsgFromBeginning(std::move(data)); // Решение подзадачи "Чтение сообщения"
}



template<>
void ParsersManager<EmptyHeader, AbstractP>::readMsgFromBeginning(std::string &&data, EmptyHeader* /*ptr*/)
{
    _currentParser = chooseParserByDataType(data.substr(2, 2));
//    if (_currentParser == nullptr)
//    {
//        qDebug() << QObject::tr("Ошибка в заголовке сообщения: невозможно определить тип данных");
//        return;
//    }
//    _currentParser->fixStartTime();
//    _currentParser->setTotalLen(data.size()); // is need?
//    _currentParser->clearCollection(); // не совсем верно для наших условий
//    _currentParser->readBlocks(std::move(data));
}

