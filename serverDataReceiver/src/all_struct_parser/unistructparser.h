#ifndef UNISTRUCTPARSER_H
#define UNISTRUCTPARSER_H
#include "abstractp.h"

template<typename T>
class UniStructParser : public AbstractP
{
public:
    UniStructParser() = default;

private:
    T _struct;
};

#endif // UNISTRUCTPARSER_H
