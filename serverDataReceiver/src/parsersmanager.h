#ifndef PARSERSMANAGER_H
#define PARSERSMANAGER_H
#include <memory>
#include <map>
#include <QDebug>
#include "dataheader.h"
#include "headerdescription.h"
#include "parsers/abstractparser.h"
/*
 * Для определенности введем терминологию:
 * - сообщение - еденица информационного обмена, существующая на уровне приложения.
 * Для реализации возможности выделения (и последующего разбора) сообщения
 * в состав сообщения добавляются префикс, длина информационной части и постфикс.
 * - tcp-дейтаграмма - "кусок" последовательности байт, полученный из tcp-потока.
 *
 * При разборе содержимого отдельной tcp-дейтаграммы возникают несколько ситуаций:
 * 1) все сообщение лежит в одной дейтаграмме. Буферизация на стороне отправителя
 * не проводилась, поэтому никаких "кусков" следующих сообщений в данном сегменте
 * не содержится. Самый простой случай.
 * 2) в одной дейтаграмме лежит несколько сообщений. Т.е. буферизация проводилась,
 * но, по случайности, сообщения разделились ровно. Случай, возникающий
 * довольно редко, т.к. tcp-протокол ничего не знает о служебных полях сообщения,
 * которые мы сами и ввели.
 * 3) в одном сегменте лежит одно или несколько сообщений, после которых идет еще
 * "кусок" следующего сообщения. Случай будет регулярно возникать, если длина
 * сообщения очень мала (100-300 байт). Причина возникновения - tcp-протокол
 * иногда буфферизирует маленькие сообщения и отправляет их в одном сегменте.
 * 4) сообщение не поместилось в один сегмент (мы считываем только его часть).
 * Самый частый случай.
*/

class Widget;
template<typename S, typename PFamily>
class ParsersManager :
        public std::enable_shared_from_this<ParsersManager<S, PFamily>>
{
private:
    void init();
    explicit ParsersManager(std::weak_ptr<Widget> w
                            , /*std::shared_ptr<S>*/ S header);
public:
    static std::shared_ptr<ParsersManager>
    create(std::weak_ptr<Widget> w, /*std::shared_ptr<S>*/ S header);

    ParsersManager(const ParsersManager& other) = delete;
    ParsersManager& operator=(const ParsersManager& other) = delete;
    ~ParsersManager();
    void parseMsg(char* dataFromTcp, int size);
    void savePieceOfData(std::string&& piece);
    void readMsgFromBeginning(std::string &&data, S* ptr = nullptr);
    std::shared_ptr<HeaderDescription<S>> headerDescription() const;
private:
    using ShPtrAbstractParser = std::shared_ptr<PFamily>;
    ShPtrAbstractParser _currentParser;
    // Два эквивалентных способа объявления указателей:
    // Важно понимать, что использование в качестве типа умного указателя
    // базового класса ЛУЧШЕ, чем использование в качестве типа
    // сложного шаблонного класса-потомка. Это связано с тем, что
    // сложный тип нуждается в двух дополнительных объявлениях классов.
    // После этих объявлений проект может просто не собраться.
//    std::shared_ptr<StructParser<SomeStruct>> _structParser;
//    ShPtrAbstractParser _structParser;
    std::map<ushort, ShPtrAbstractParser> _dataParsers;
    std::weak_ptr<Widget> _widget;
    std::shared_ptr<HeaderDescription<S>> _header;
    std::string _pieceOfData{};
    ShPtrAbstractParser chooseParserByDataType(ushort type);
};

//#include <exception>
ushort convert(const std::string& type)
{
    if (type.size() != sizeof(ushort))
//        throw std::bad...
        return 0;
    ushort id;
    memcpy(&id, type.data(), sizeof(id));
    return id;
}

template<>
void ParsersManager<DataHeader, AbstractParser>::readMsgFromBeginning(std::string &&data, DataHeader* /*ptr*/)
{   // Проверяем сообщение на наличие префикса:
    if (_header->prefixPos(data) == 0)
    {
        auto type = convert(data.substr(2, 2));
        _currentParser = chooseParserByDataType(type);
        if (_currentParser == nullptr)
        {
            qDebug() << QObject::tr("Ошибка в заголовке сообщения: невозможно определить тип данных");
            return;
        }
        _currentParser->fixStartTime();
        // Извлекаем длину:
//        auto totalLen = getLen(data);
        auto totalLen = _header->getLen(data);
        _currentParser->setTotalLen(totalLen);
        qDebug() << "GET NEW DATA LEN from real packet: " << totalLen;
        data.erase(0, 12);

        _currentParser->clearCollection();
    }
    if (_currentParser == nullptr)
    {   // В общем случае это условие не должно выполняться:
        qDebug() << QObject::tr("Активный парсер почему-то не выбран. Разбор сообщения прерван.");
        return;
    }
    _currentParser->readBlocks(std::move(data));
}

#include "parsersmanager_impl.h"
#endif // PARSERSMANAGER_H

