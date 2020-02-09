#ifndef STRUCTPARSER_H
#error "Do not include this file directly! Use structparser.h!"
#endif

#include <QDebug>
#include <QObject>
#include <netinet/in.h>
#include "parsersmanager.h"


template<typename T>
StructParser<T>::StructParser(std::weak_ptr<ParsersManager<DataHeader> > pm)
    : AbstractParser(pm)
{
    _structObjects.reserve(12000);
}

template<typename T>
StructParser<T>::~StructParser()
{
    qDebug() << "StructParser::~dtor() called";
}

template<typename T>
void StructParser<T>::clearCollection()
{
    _structObjects.clear();
}

template<typename T>
void StructParser<T>::createObject(std::string &data, size_t pos)
{
    _oneObjectSerializingTimer->fixStartTime();
    auto obj = data.substr(4, pos);
    T testStruct{};
    memcpy(&testStruct, &obj[0], pos);
    _structObjects.push_back(testStruct);
    _oneObjectSerializingTimer->fixEndTime();

    data.erase(0, pos + 4);
    _totalLen = _totalLen - (pos + 4);

    if (_totalLen == 0 && data.substr(0, 2)
            == _parsersManager.lock()->headerDescription()->postfixStr())
    {
        data.erase(0, 2);
    }
}

template<typename T>
void StructParser<T>::readBlocks(std::string &&data)
{
    while (_totalLen != 0)
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
                _parsersManager.lock()->savePieceOfData(std::move(data));
                return;
            }
            createObject(data, len);
        }
        else {
            qDebug() << QObject::tr("Количество оставшихся байт < 4. Сохраняем их.");
            _parsersManager.lock()->savePieceOfData(std::move(data));
            return;
        }
    }
    if (_totalLen == 0) // На всякий случай
    {
        _wholeMessageParsingTimer->fixEndTime();
        double objAverageTime = _oneObjectSerializingTimer->getAverage();
        double wholeMessageTime = _wholeMessageParsingTimer->getAverage();
        emit messageParsingComplete(MessageParsingResult{DataType::Struct
                                                         , _structObjects.size()
                                                         , wholeMessageTime
                                                         , objAverageTime});
        qDebug() << "-- _totalLen = " << _totalLen;
        qDebug() << "-- Structs amount = " << _structObjects.size();

        if (!data.empty())
            _parsersManager.lock()->readMsgFromBeginning(std::move(data));
    }
}

