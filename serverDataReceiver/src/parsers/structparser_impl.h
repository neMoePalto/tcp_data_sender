#ifndef STRUCTPARSER_H
#error "Do not include this file directly! Use structparser.h!"
#endif

#include <QDebug>
#include <QObject>
#include <netinet/in.h>
#include "parsersmanager.h"


template<typename T, typename S>
StructParser<T,S>::StructParser(std::weak_ptr<ParsersManager<S>> pm)
    : AbstractParser<S>(pm)
{
    _structObjects.reserve(12000);
}

template<typename T, typename S>
StructParser<T,S>::~StructParser()
{
    qDebug() << "StructParser::~dtor() called";
}

template<typename T, typename S>
void StructParser<T,S>::clearCollection()
{
    _structObjects.clear();
}

template<typename T, typename S>
void StructParser<T,S>::createObject(std::string &data, size_t pos)
{
    AbstractParser<S>::_oneObjectSerializingTimer->fixStartTime();
    auto obj = data.substr(4, pos);
    T testStruct{};
    memcpy(&testStruct, &obj[0], pos);
    _structObjects.push_back(testStruct);
    AbstractParser<S>::_oneObjectSerializingTimer->fixEndTime();

    data.erase(0, pos + 4);
    AbstractParser<S>::_totalLen = AbstractParser<S>::_totalLen - (pos + 4);

    if (AbstractParser<S>::_totalLen == 0 && data.substr(0, 2)
            == AbstractParser<S>::_parsersManager.lock()->headerDescription()->postfixStr())
    {
        data.erase(0, 2);
    }
}

template<typename T, typename S>
void StructParser<T,S>::readBlocks(std::string &&data)
{
    while (AbstractParser<S>::_totalLen != 0)
    {
        if (data.size() >= 4) // т.к. sizeof(len) == 4
        {
            // Считываем значение длины одной структуры.
            // TODO: в случае, если значение длины не содержится в принятом сообщении,
            // его можно задать константой. При этом теряется возможность (теоретическая)
            // отправлять в одном сообщении кодограммы разной длины.
            uint len;
            memcpy(&len, &data[0], sizeof(len));
            len = htonl(len);
            // Если вторая часть рассматриваемого блока данных находится за пределами
            // текущего сообщения:
            if (len + sizeof(len) > data.size())
            {
//                qDebug() << tr("Вторая часть рассматриваемого блока данных "
//                               "находится за пределами текущего сообщения.");
                AbstractParser<S>::_parsersManager.lock()->savePieceOfData(std::move(data));
                return;
            }
            createObject(data, len);
        }
        else {
            qDebug() << QObject::tr("Количество оставшихся байт < 4. Сохраняем их.");
            AbstractParser<S>::_parsersManager.lock()->savePieceOfData(std::move(data));
            return;
        }
    }
    if (AbstractParser<S>::_totalLen == 0) // На всякий случай
    {
        AbstractParser<S>::_wholeMessageParsingTimer->fixEndTime();
        double objAverageTime = AbstractParser<S>::_oneObjectSerializingTimer->getAverage();
        double wholeMessageTime = AbstractParser<S>::_wholeMessageParsingTimer->getAverage();
        emit AbstractParser<S>::messageParsingComplete(MessageParsingResult{DataType::Struct
                                                         , _structObjects.size()
                                                         , wholeMessageTime
                                                         , objAverageTime});
        qDebug() << "-- _totalLen = " << AbstractParser<S>::_totalLen;
        qDebug() << "-- Structs amount = " << _structObjects.size();

        if (!data.empty())
            AbstractParser<S>::_parsersManager.lock()->readMsgFromBeginning(std::move(data));
    }
}

