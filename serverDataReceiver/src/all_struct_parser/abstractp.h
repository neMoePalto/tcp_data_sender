#ifndef ABSTRACTP_H
#define ABSTRACTP_H
#include <string>

class AbstractP
{
public:
    AbstractP() = default;
    virtual ~AbstractP() = default;
    virtual void initStruct(std::string& data, uint len) = 0;
    virtual void clearCollection() = 0;
    // May be temp:
    virtual void useData() = 0;
//    virtual std::vector<T> getData() = 0; - а параметр-то шаблонный )
};

#endif // ABSTRACTP_H
