#ifndef JSONPARSER_H
#error "Do not include this file directly! Use jsonparser.h!"
#endif

//#include "jsonparser.h"
#include <boost/property_tree/json_parser.hpp>
#include "parsersmanager.h"

template<typename S>
JsonParser<S>::JsonParser(std::weak_ptr<ParsersManager<S>> pm)
    : AbstractParser<S>(pm)
{
    _delimiterJsonPostfix = "}\n";
    _delimiterJsonPostfix +=
            AbstractParser<S>::_parsersManager.lock()->headerDescription()->postfixStr();
    // Резервирую место для большого количества объектов.
    // Не самое оптимальное решение, но свою пользу оно приносит:
    _jsonObjects.reserve(12000);
}

template<typename S>
JsonParser<S>::~JsonParser()
{
    qDebug() << "JsonParser::~dtor() called";
}

template<typename S>
void JsonParser<S>::clearCollection()
{
    _jsonObjects.clear();
}

template<typename S>
void JsonParser<S>::createObject(std::string& data, size_t posEnd)
{
    size_t pos = posEnd + 1;
    AbstractParser<S>::_oneObjectSerializingTimer->fixStartTime();
    auto obj = data.substr(0, pos);
    ptree tree;
    // Перенести 1 строку за пределы расчетной области:
    std::stringstream iss(std::move(obj));
    boost::property_tree::json_parser::read_json(iss, tree);
    _jsonObjects.push_back(tree);
    AbstractParser<S>::_oneObjectSerializingTimer->fixEndTime();

    data.erase(0, pos);
    AbstractParser<S>::_totalLen = AbstractParser<S>::_totalLen - pos;
}

template<typename S>
void JsonParser<S>::readBlocks(std::string &&data)
{   // Сериализуем json-объекты (все, кроме последнего):
    while (true)
    {
        size_t jsonEnd = data.find(_delimiterJsonJson);
        if (jsonEnd != std::string::npos)
        {   // Мы "нашли" конец 1 блока, но в следующем сообщении
            if (jsonEnd > AbstractParser<S>::_totalLen)
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
        AbstractParser<S>::_totalLen = AbstractParser<S>::_totalLen - 1;
//        qDebug() << "_totalLen = " << _totalLen << "\ndata.size() = " << data.size();
//        qDebug() << "JSON-objects amount = " << _jsonObjects.size();

        AbstractParser<S>::_wholeMessageParsingTimer->fixEndTime();
        double objAverageTime = AbstractParser<S>::_oneObjectSerializingTimer->getAverage();
        double wholeMessageTime = AbstractParser<S>::_wholeMessageParsingTimer->getAverage();
//        qDebug() << QTextStream::ShowBase << "Average:" << objAverageTime
//                 << "sec, and wholeMsgTime:" << wholeMessageTime << "sec";
        emit AbstractParserSignalsSlots::
                messageParsingComplete(MessageParsingResult{DataType::Json
                                                         , _jsonObjects.size()
                                                         , wholeMessageTime
                                                         , objAverageTime});
        // В общем случае это условие не должно выполняться:
        if (AbstractParser<S>::_totalLen  != 0)
        {
            QString s(QObject::tr("Разбор сообщения завершен, но длина почему-то не равна нулю."));
            s = s + " _totalLen = " + QString::number(AbstractParser<S>::_totalLen);
            qDebug() << s;
            emit AbstractParser<S>::parsingError(s);
            AbstractParser<S>::_totalLen = 0;
        }
        // Если начало следующего сообщения находится в текущей дейтаграмме:
        if (!data.empty())
            AbstractParser<S>::_parsersManager.lock()->readMsgFromBeginning(std::move(data)); // Переход к решению подзадачи "Чтение сообщения"
    }
    else
    {
        size_t jsonEnd =
                AbstractParser<S>::_parsersManager.lock()->headerDescription()->prefixPos(data);
//                _header->prefixPos(data);
        if (jsonEnd != std::string::npos)
        {   // В общем случае это условие не должно выполняться:
            QString s(QObject::tr("Удаляем мусор, расположенный перед префиксом нового сообщения."
                         "\nРазмер нечитаемого сообщения = "));
            s = s + QString::number(jsonEnd);
            qDebug() << s;
            emit AbstractParser<S>::parsingError(s);
            data.erase(0, jsonEnd);
            AbstractParser<S>::_parsersManager.lock()->readMsgFromBeginning(std::move(data)); // Переход к решению подзадачи "Чтение сообщения"
        }
        else { // Сохраняем "кусок сообщения", ожидаем следующую дейтаграмму:
            AbstractParser<S>::_parsersManager.lock()->savePieceOfData(std::move(data));
        }
    }
}

