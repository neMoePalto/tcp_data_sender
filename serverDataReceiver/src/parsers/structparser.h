#ifndef STRUCTPARSER_H
#define STRUCTPARSER_H
#include "abstractparser.h"
#include <vector>

template<typename T, typename S>
class StructParser : public AbstractParser<S>
{
public:
//    using AbstractParser<S>::AbstractParser;
    StructParser(std::weak_ptr<ParsersManager<S>> p
                 , std::shared_ptr<S> header);
    ~StructParser() override;
    void clearCollection() override;
    void createObject(std::string &data, size_t pos) override;
    void readBlocks(std::string &&data) override;
private:
    std::vector<T> _structObjects{};
};

#include "structparser_impl.h"
#endif // STRUCTPARSER_H
