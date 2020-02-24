#ifndef ABSTRACTP_H
#define ABSTRACTP_H
#include <string>

class AbstractP
{
public:
    AbstractP(ushort num)
        : _num(num) {
    }
    virtual ~AbstractP() = default;
    virtual void initStruct(std::string& data, uint len) = 0;
    virtual void clearCollection() = 0;
    // Простой вариант обработки сериализованных объектов:
//    virtual void useData() = 0;
    // Проблема в том, что виртуальная функция не может быть шаблонной:
//    virtual std::vector<T> getData() = 0;
    ushort getNum() {
        return _num;
    }
protected:
    ushort _num;
};

#endif // ABSTRACTP_H
