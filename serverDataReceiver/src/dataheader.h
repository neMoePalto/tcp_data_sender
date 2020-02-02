#ifndef DATAHEADER_H
#define DATAHEADER_H
#include <sys/types.h>
#include <vector>

struct DataHeader
{
    ushort  prefix;
    ushort  type;
    ulong   len;
    ushort  postfix;
};

struct EmptyHeader
{};

#endif // DATAHEADER_H
