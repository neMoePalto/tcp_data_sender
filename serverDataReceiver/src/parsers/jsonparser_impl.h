#ifndef JSONPARSER_H
#error "Do not include this file directly! Use jsonparser.h!"
#endif

#include <boost/property_tree/json_parser.hpp>
#include "parsersmanager.h"
#include "headerdescription.h"


JsonParser::JsonParser(std::weak_ptr<ParsersManager<DataHeader>> pm)
    : AbstractParser(pm)
{
    _delimiterJsonPostfix = "}\n";
    _delimiterJsonPostfix +=
            _parsersManager.lock()->headerDescription()->postfixStr();
    // Резервирую место для большого количества объектов.
    // Не самое оптимальное решение, но свою пользу оно приносит:
    _jsonObjects.reserve(12000);
}

JsonParser::~JsonParser()
{
    qDebug() << "JsonParser::~dtor() called";
}

void JsonParser::clearCollection()
{
    _jsonObjects.clear();
}

void JsonParser::createObject(std::string& data, size_t posEnd)
{
    size_t pos = posEnd + 1;
    _oneObjectSerializingTimer->fixStartTime();
    auto obj = data.substr(0, pos);
    ptree tree;
    // Перенести 1 строку за пределы расчетной области:
    std::stringstream iss(std::move(obj));
    boost::property_tree::json_parser::read_json(iss, tree);
    _jsonObjects.push_back(tree);
    _oneObjectSerializingTimer->fixEndTime();

    data.erase(0, pos);
    _totalLen = _totalLen - pos;
}

void JsonParser::readBlocks(std::string &&data)
{   // Сериализуем json-объекты (все, кроме последнего):
    while (true)
    {
        size_t jsonEnd = data.find(_delimiterJsonJson);
        if (jsonEnd != std::string::npos)
        {   // Мы "нашли" конец 1 блока, но в следующем сообщении
            if (jsonEnd > _totalLen)
                break;
            createObject(data, jsonEnd);
        }
        else
            break;
    }
    // -------------------------------------------------
    size_t jsonEnd = data.find(_delimiterJsonPostfix);
    if (jsonEnd != std::string::npos)
    {   // Перед нами - последний json-объект нашего сообщения:
        createObject(data, jsonEnd);
        // Разбор сообщения завершен. Удаляем его "остатки":
        data.erase(0, 3);
        _totalLen = _totalLen - 1;
//        qDebug() << "_totalLen = " << _totalLen << "\ndata.size() = " << data.size();
//        qDebug() << "JSON-objects amount = " << _jsonObjects.size();

        _wholeMessageParsingTimer->fixEndTime();
        double objAverageTime = _oneObjectSerializingTimer->getAverage();
        double wholeMessageTime = _wholeMessageParsingTimer->getAverage();
//        qDebug() << QTextStream::ShowBase << "Average:" << objAverageTime
//                 << "sec, and wholeMsgTime:" << wholeMessageTime << "sec";
        emit AbstractParserSignalsSlots::
                messageParsingComplete(MessageParsingResult{DataType::Json
                                                         , _jsonObjects.size()
                                                         , wholeMessageTime
                                                         , objAverageTime});
        // В общем случае это условие не должно выполняться:
        if (_totalLen  != 0)
        {
            QString s(QObject::tr("Разбор сообщения завершен, но длина почему-то не равна нулю."));
            s = s + " _totalLen = " + QString::number(_totalLen);
            qDebug() << s;
            emit parsingError(s);
            _totalLen = 0;
        }
        // Если начало следующего сообщения находится в текущей дейтаграмме:
        if (!data.empty())
            _parsersManager.lock()->readMsgFromBeginning(std::move(data)); // Переход к решению подзадачи "Чтение сообщения"
    }
    else
    {
        size_t jsonEnd = _parsersManager.lock()->headerDescription()->prefixPos(data);
        if (jsonEnd != std::string::npos)
        {   // В общем случае это условие не должно выполняться:
            QString s(QObject::tr("Удаляем мусор, расположенный перед префиксом нового сообщения."
                         "\nРазмер нечитаемого сообщения = "));
            s = s + QString::number(jsonEnd);
            qDebug() << s;
            emit parsingError(s);
            data.erase(0, jsonEnd);
            _parsersManager.lock()->readMsgFromBeginning(std::move(data)); // Переход к решению подзадачи "Чтение сообщения"
        }
        else { // Сохраняем "кусок сообщения", ожидаем следующую дейтаграмму:
            _parsersManager.lock()->savePieceOfData(std::move(data));
        }
    }
}

