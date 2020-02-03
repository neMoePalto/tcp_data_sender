#ifndef ABSTRACTPARSER_H
#define ABSTRACTPARSER_H
#include <QString>
#include <string>
#include <memory>
#include "averagetime.h"
//#include "parsersmanager.h"
#include "abstractparsersignalsslots.h"

template<typename S>
class ParsersManager;

template<typename S>
class AbstractParser : public AbstractParserSignalsSlots
{
public:
    AbstractParser(std::weak_ptr<ParsersManager<S>> p
                   , std::shared_ptr<S> header);
    virtual ~AbstractParser() = default;
    virtual void clearCollection() = 0;
    virtual void createObject(std::string &data, size_t posEnd) = 0;
    virtual void readBlocks(std::string &&data) = 0;
    void setTotalLen(ulong len);
protected:
    std::shared_ptr<AverageTime> _oneObjectSerializingTimer;
    std::shared_ptr<AverageTime> _wholeMessageParsingTimer;
    std::weak_ptr<ParsersManager<S>> _parsersManager{};
    std::shared_ptr<S> _header;
//    readMsgFromBeginning(std::string &&data, S* ptr);

    ulong _totalLen{0};
};

#include "abstractparser_impl.h"
#endif // ABSTRACTPARSER_H
