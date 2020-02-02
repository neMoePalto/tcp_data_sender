#ifndef HEADERDESCRIPTION_H
#define HEADERDESCRIPTION_H
#include <string>

struct DataHeader;

template<typename S>
class HeaderDescription
{
public:
//    HeaderDescription(S&& header);
    HeaderDescription(DataHeader header);
    ~HeaderDescription();
    ulong getLen(std::string& currentDataBlock);
    size_t prefixPos(const std::string& currentDataBlock) const;
    std::string postfixStr() const;
    uint emptyMsgLen() const {
        return _emptyMsgLen;
    }
private:
    const S _header;
    //    const uint _emptyMsgLen = 14;
    uint _emptyMsgLen{};
    std::string _prefixStr{};
    std::string _postfixStr{};
};
using HdrDescrDH = HeaderDescription<DataHeader>;

#include "headerdescription_impl.h"
#endif // HEADERDESCRIPTION_H
