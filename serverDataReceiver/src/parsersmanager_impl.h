#ifndef PARSERSMANAGER_H
#error "Do not include this file directly! Use parsersmanager.h!"
#endif

#include "parsers/jsonparser.h"
#include "parsers/structparser.h"
#include "widget.h"
#include "somestruct.h"
#include <netinet/in.h>
#include <QDebug>
#include <QObject>
#include "headerdescription.h"

template<typename S>
ParsersManager<S>::ParsersManager(Widget *w, std::shared_ptr<S> header)
    : _widget(w)
    , _header(header)
{
//    _jsonParser   = std::make_shared<JsonParser<DataHeader>>(this);
//    _structParser = std::make_shared<StructParser<SomeStruct, DataHeader>>(this);
    auto _jsonParser   = std::make_shared<JsonParser<
            HeaderDescription<DataHeader>>>(this, _header);
    auto _structParser = std::make_shared<StructParser<SomeStruct,
            HeaderDescription<DataHeader>>>(this, _header);

    // 2 байта, определяющие тип передаваемых данных:
    _dataParsers.insert({"JJ", _jsonParser});
    _dataParsers.insert({"SS", _structParser});

    // Связываем сигналы объектов производных классов со слотами:
    QObject::connect(_jsonParser.get(), SIGNAL(messageParsingComplete(MessageParsingResult)),
                     _widget,           SLOT(printParsingResults(MessageParsingResult)) );
    QObject::connect(_structParser.get(), SIGNAL(messageParsingComplete(MessageParsingResult)),
                     _widget,             SLOT(printParsingResults(MessageParsingResult)) );
}


template<typename S>
void ParsersManager<S>::savePieceOfData(std::string&& piece)
{
    _pieceOfData = std::move(piece);
}

template<typename S>
typename ParsersManager<S>::ShPtrAbstractParser
ParsersManager<S>::chooseParserByDataType(const std::string& type)
{
    auto it = _dataParsers.find(type);
    if (it == _dataParsers.end())
        return nullptr;
    else {
        return (*it).second;
    }
}

template<typename S>
void ParsersManager<S>::parseMsg(char* dataFromTcp, int size)
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

template<typename S>
void ParsersManager<S>::readMsgFromBeginning(std::string &&data)
{   // Проверяем сообщение на наличие префикса:
    if (_header->prefixPos(data) == 0)
    {
        _currentParser = chooseParserByDataType(data.substr(2, 2));
        if (_currentParser == nullptr)
        {
            qDebug() << QObject::tr("Ошибка в заголовке сообщения: невозможно определить тип данных");
            return;
        }
        _currentParser->_wholeMessageParsingTimer->fixStartTime();
        // Извлекаем длину:
//        auto totalLen = getLen(data);
        auto totalLen = _header->getLen(data);
        _currentParser->setTotalLen(totalLen);
        qDebug() << "GET NEW DATA LEN from real packet: " << totalLen;
        data.erase(0, 12);

        _currentParser->clearCollection();
    }
    if (_currentParser == nullptr)
    {   // В общем случае это условие не должно выполняться:
        qDebug() << QObject::tr("Активный парсер почему-то не выбран. Разбор сообщения прерван.");
        return;
    }
    _currentParser->readBlocks(std::move(data));
}

