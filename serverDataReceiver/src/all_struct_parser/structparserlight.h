#ifndef STRUCTPARSERLIGHT_H
#define STRUCTPARSERLIGHT_H
#include "abstractp.h"
#include <sys/types.h>
#include <vector>

struct DataOne
{
    char a;
    int b;
};

struct DataTwo
{
    int a;
    int b;
    short c;
};

template<typename T>
class StructParserLight : public AbstractP
{
public:
    StructParserLight(ushort num); // Если номер нужен
    void initStruct(std::string& data, uint len) override;
    void clearCollection() override;
    void useData() override;
    // Experimental:
    std::vector<T> getData();

private:
    std::vector<T> _structs;
    ushort _num;
};

#include "structparserlight_impl.h"
#endif // UNISTRUCTPARSER_H
