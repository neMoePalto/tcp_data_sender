#ifndef ABSTRACTPARSER_H
#define ABSTRACTPARSER_H
#include <QString>
#include <string>
#include <memory>
#include "averagetime.h"
#include "abstractparsersignalsslots.h"

template<typename S, typename PFamily>
class ParsersManager;
struct DataHeader;

class AbstractParser : public AbstractParserSignalsSlots
{
public:
    AbstractParser(std::weak_ptr<ParsersManager<DataHeader, AbstractParser>> pm);
    virtual ~AbstractParser() = default;
    virtual void clearCollection() = 0;
    virtual void createObject(std::string &data, size_t posEnd) = 0;
    virtual void readBlocks(std::string &&data) = 0;
    void setTotalLen(ulong len);
    void fixStartTime();
protected:
    std::shared_ptr<AverageTime> _oneObjectSerializingTimer;
    std::shared_ptr<AverageTime> _wholeMessageParsingTimer;
    std::weak_ptr<ParsersManager<DataHeader, AbstractParser>> _parsersManager{};
    ulong _totalLen{0};
};

#include "abstractparser_impl.h"
#endif // ABSTRACTPARSER_H
