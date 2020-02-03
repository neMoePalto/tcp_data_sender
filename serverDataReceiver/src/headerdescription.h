#ifndef HEADERDESCRIPTION_H
#define HEADERDESCRIPTION_H
#include <string.h>
#include <string>
#include <algorithm>

struct DataHeader;
struct EmptyHeader;

template<typename S>
class HeaderDescription
{
public:
//    HeaderDescription(S&& header);
    HeaderDescription(DataHeader header);
    HeaderDescription(EmptyHeader header);
    ~HeaderDescription();

    template<typename = std::enable_if_t<std::is_same<S, DataHeader>::value>>
    ulong getLen(std::string &currentDataBlock)
    {
        ulong len{};
        uint pos = 4;
        std::reverse(currentDataBlock.begin() + pos,
                     currentDataBlock.begin() + pos + 8);
        memcpy(&len, &currentDataBlock[pos], 8);
        return len;
    }

    size_t prefixPos(const std::string& currentDataBlock) const;
    std::string postfixStr() const;
    uint emptyMsgLen() const
    {
        return _emptyMsgLen;
    }
private:
    const S _header;
    //    const uint _emptyMsgLen = 14;
    uint _emptyMsgLen{0};
    std::string _prefixStr{};
    std::string _postfixStr{};
};


#include "headerdescription_impl.h"
#endif // HEADERDESCRIPTION_H
