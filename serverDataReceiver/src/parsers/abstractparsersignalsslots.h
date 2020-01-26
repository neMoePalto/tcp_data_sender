#ifndef ABSTRACTPARSERSIGNALSSLOTS_H
#define ABSTRACTPARSERSIGNALSSLOTS_H
#include <QObject>

enum class DataType
{
    Json,
    Struct
};

using MessageParsingResult = std::tuple<DataType, ulong, double, double>;
class AbstractParserSignalsSlots : public QObject
{
    Q_OBJECT
public:
    AbstractParserSignalsSlots() = default;
signals:
    void messageParsingComplete(MessageParsingResult);
    void parsingError(QString); // Unused now.
};

#endif // ABSTRACTPARSERSIGNALSSLOTS_H
