#ifndef HEADERDESCRIPTION_H
#error "Do not include this file directly! Use headerdescription.h!"
#endif

#include <algorithm>
#include <netinet/in.h>
#include <QDebug>
#include "headerdescription.h"
#include "structs/dataheader.h"

//template<typename S>
//HeaderDescription<S>::HeaderDescription(S&& header)
//    : _header(std::forward<DataHeader>(header))
//{}

template<typename S>
HeaderDescription<S>::HeaderDescription(DataHeader header)
    : _header(header)
{
    // Корректный способ посчитать sizeof структуры:
    _emptyMsgLen = sizeof(header.prefix) + sizeof(header.type)
            + sizeof(header.len) + sizeof(header.postfix);
    // Подготовка последовательностей символов для контроля начала и
     // конца сообщения в формате std::string :
    auto pref = htons(header.prefix);
//    _prefixStr.resize(2);
    uint sz = sizeof(header.prefix);
    _prefixStr.resize(sz);
    memcpy(&_prefixStr[0], &pref, sz);

    auto postf = htons(header.postfix);
    sz = sizeof(header.postfix);
    _postfixStr.resize(sz);
    memcpy(&_postfixStr[0], &postf, sz);

    qDebug() << "Sizes:" << _emptyMsgLen
             << "," << sizeof(header.prefix)
             << "and" << sizeof(header.postfix);
}

template<typename S>
HeaderDescription<S>::~HeaderDescription()
{
    qDebug() << "HeaderDescription<S>::~dtor() called";
}

template<typename S>
size_t HeaderDescription<S>::prefixPos(const std::string &currentDataBlock) const
{
    return currentDataBlock.find(_prefixStr);
}

template<typename S>
std::string HeaderDescription<S>::postfixStr() const
{
    return _postfixStr;
}

template<>
ulong HeaderDescription<DataHeader>::getLen(std::string &currentDataBlock)
{
    ulong len{};
    uint pos = 4;
    std::reverse(currentDataBlock.begin() + pos,
                 currentDataBlock.begin() + pos + 8);
    memcpy(&len, &currentDataBlock[pos], 8);
    return len;
}

//template<>
//ulong HeaderDescription<EmptyHeader>::getLen(std::string &currentDataBlock)
//{
//    return 15;
//}

//---------------------------------------------
template<typename S>
HeaderDescription<S>::HeaderDescription(EmptyHeader header)
    : _header(header)
{
}

