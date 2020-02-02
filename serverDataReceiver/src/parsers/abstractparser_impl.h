#ifndef ABSTRACTPARSER_H
#error "Do not include this file directly! Use abstractparser.h!"
#endif

#include "abstractparser.h"

template<typename S>
AbstractParser<S>::AbstractParser(std::weak_ptr<ParsersManager<S>> p
                                  , std::shared_ptr<S> header)
    : _parsersManager(p)
    , _header(header)
{
    _oneObjectSerializingTimer = std::make_shared<AverageTime>();
    _wholeMessageParsingTimer  = std::make_shared<AverageTime>();
}

template<typename S>
void AbstractParser<S>::setTotalLen(ulong len)
{
    _totalLen = len;
}

template<typename S>
void AbstractParser<S>::fixStartTime()
{
    _wholeMessageParsingTimer->fixStartTime();
}

