#ifndef DATAHANDLER_H
#error "Do not include this file directly! Use datahandler.h!"
#endif

#include "datahandler.h"
#include <QDebug>

struct kd_fromT4_01a;
struct DataOne;
struct DataTwo;

template<typename T, typename PFamily>
void DataHandler::handle(const std::map<ushort, std::shared_ptr<PFamily>> *dataParsers)
{   // Чтобы не заводить дополнительных шаблонных функций (в клиентском коде),
    // предусмотрена возможность передачи в данный метод nullptr на коллекцию парсеров.
    if (dataParsers == nullptr)
        return;
//    qDebug() << "   handler() ===>";
    for (auto &obj : *dataParsers)
    {
        auto sp = obj.second;
        auto parserLight = std::dynamic_pointer_cast<StructParserLight<T>>(sp);
        if (parserLight == nullptr)
        {
            qDebug() << "nullptr";
            continue;
        }
        else {
            qDebug() << "Real ptr!";
            auto vec = parserLight->getData();
            processData<T>(vec);
            return;
        }
    }
}

template<typename T>
void DataHandler::processData(std::vector<T>& data)
{
    for (auto &obj : data)
        processObj(obj);

    data.clear();
}

// Необходимо написать специализации для всех типов данных:
template<>
void DataHandler::processObj(kd_fromT4_01a& /*obj*/)
{
    qDebug() << "specialization for kd_97L6_01a";
}

template<>
void DataHandler::processObj(DataOne& /*obj*/)
{
    qDebug() << "\tspecialization for DataOne";
}

template<>
void DataHandler::processObj(DataTwo& /*obj*/)
{
    qDebug() << "specialization for DataTwo";
}
