#ifndef STRUCTPARSER_H
#define STRUCTPARSER_H
#include "abstractparser.h"
#include <vector>

template<typename T>
class StructParser : public AbstractParser
{
public:
//    using AbstractParser<S>::AbstractParser;
    StructParser(std::weak_ptr<ParsersManager<DataHeader, AbstractParser>> pm);
    ~StructParser() override;
    void clearCollection() override;
    void createObject(std::string &data, size_t pos) override;
    void readBlocks(std::string &&data) override;
private:
    std::vector<T> _structObjects{};
};

#include "structparser_impl.h"
#endif // STRUCTPARSER_H
