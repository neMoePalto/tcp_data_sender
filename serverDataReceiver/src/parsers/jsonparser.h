#ifndef JSONPARSER_H
#define JSONPARSER_H
#include "abstractparser.h"
#include <QDebug>
#include <vector>
#include <boost/property_tree/ptree.hpp>
using namespace boost::property_tree;

template<typename S>
class JsonParser : public AbstractParser<S>
{
public:
    // Подход, позволяющий не объявлять и не определять "пустой" конструктор
    // в производном классе. Идеально для случая, когда задача такого ctor'a
    // сводится к транизиту параметоров в ctor базового класса:
//    using AbstractParser::AbstractParser;
    JsonParser(std::weak_ptr<ParsersManager<S>> pm);
    ~JsonParser() override;
    void clearCollection() override;
    void createObject(std::string &data, size_t posEnd) override;
    void readBlocks(std::string &&data) override;
private:
    const std::string _delimiterJsonJson{ "}\n{" };
    std::string       _delimiterJsonPostfix{};
    std::vector<ptree> _jsonObjects{};
};

#include "jsonparser_impl.h"
#endif // JSONPARSER_H
