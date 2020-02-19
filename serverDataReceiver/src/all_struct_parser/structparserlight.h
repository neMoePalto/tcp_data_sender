#ifndef STRUCTPARSERLIGHT_H
#define STRUCTPARSERLIGHT_H
#include "abstractp.h"
#include <sys/types.h>
#include <vector>

// Примеры структур:
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
    // Простой вариант обработки объектов типа T:
    // реализовать в каждой специализации класса этот метод.
    // Недостаток такого подхода: и сериализация объектов,
    // и их обработка происходят в одном классе:
//    void useData() override;
    std::vector<T> getData();
private:
    std::vector<T> _structs;
};

#include "structparserlight_impl.h"
#endif // UNISTRUCTPARSER_H
