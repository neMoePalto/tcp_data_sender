#ifndef STRUCTPARSERLIGHT_H
#error "Do not include this file directly! Use structparser.h!"
#endif

#include "structparserlight.h"
#include "structs/kdfrom_ksa4.h"
#include <QDebug>

template<typename T>
StructParserLight<T>::StructParserLight(ushort num)
    : _num(num)
{
}

template<typename T>
void StructParserLight<T>::initStruct(std::string& data, uint len)
{
    T myStruct;
    memcpy(&myStruct, data.data(), len);
    data.erase(0, len);
    _structs.push_back(myStruct);
    qDebug() << "sizeof(T)=" << sizeof (T);
}

template<typename T>
void StructParserLight<T>::clearCollection()
{
    _structs.clear();
}

template<typename T>
void StructParserLight<T>::useData()
{
    _structs.clear();
}

template<>
void StructParserLight<kd_97L6_01a>::useData()
{
    qDebug() << "\n";
    for (auto &obj : _structs)
    {
        // Обработка структуры:
        qDebug() << "Номер цели в сист. КСА:" << obj.ntr << "\n"
                 << "Широта:" << double(obj.bl) << ", Долгота:" << double(obj.dl) << "\n"
                 << "Высота:" << obj.h << ", Скорость гориз.:" << obj.vp << ", Пр. бедствия:" << obj.bd;
    }
    _structs.clear();
}

template<>
void StructParserLight<DataOne>::useData()
{
    qDebug() << "Test impl for struct DataOne";
}


template<typename T>
std::vector<T> StructParserLight<T>::getData()
{
    return _structs;
}
