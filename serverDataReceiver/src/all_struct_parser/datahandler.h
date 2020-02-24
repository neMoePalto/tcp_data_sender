#ifndef DATAHANDLER_H
#define DATAHANDLER_H
#include <map>
#include <vector>
#include <memory>
#include "abstractp.h"

template<typename T>
class StructParserLight;

class DataHandler
{
public:
    DataHandler() = default;
    template<typename T, typename PFamily>
    void handle(const std::map<ushort, std::shared_ptr<PFamily>>* dataParsers);
private:
    template<typename T>
    void processData(std::vector<T>& data);
    template<typename T>
    void processObj(T& obj);// {}
};

#include "datahandler_imlp.h"
#endif // DATAHANDLER_H
