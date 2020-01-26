#ifndef ABSTRACTPARSER_H
#define ABSTRACTPARSER_H
#include <QString>
#include <string>
#include <memory>
#include "averagetime.h"
#include "parsersmanager.h"
#include "abstractparsersignalsslots.h"


template<typename S>
class AbstractParser : public AbstractParserSignalsSlots
{
public:
    AbstractParser(ParsersManager<S>* p
                   , std::shared_ptr<S> header);
    virtual ~AbstractParser() = default;
    virtual void clearCollection() = 0;
    virtual void createObject(std::string &data, size_t posEnd) = 0;
    virtual void readBlocks(std::string &&data) = 0;
    void setTotalLen(ulong len);
protected:
    std::shared_ptr<AverageTime> _oneObjectSerializingTimer;
    std::shared_ptr<AverageTime> _wholeMessageParsingTimer;
    ParsersManager<S>* _parsersManager = nullptr;
    std::shared_ptr<S> _header;
    friend void ParsersManager<S>::readMsgFromBeginning(std::string &&data);

    ulong _totalLen{0};
};

#include "abstractparser_impl.h"
#endif // ABSTRACTPARSER_H
