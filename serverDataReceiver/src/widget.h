#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include <QList>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QTableWidget>
#include "restarter.h"
#include "parsersmanager.h"
#include "parsers/abstractparser.h"
#include "headerdescription.h"

namespace MySpace {
const int a = 4;
const char b = 'b';
}

struct Temp
{
    int a;
    char b;
};


enum class ClearLabelsPolicy
{
    All,
    ExceptFirst
};

class QTime;
class DataHeader;
class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget();
    ~Widget();
    using ptrFnProcessing = void(Widget::*)(QByteArray ba);

public slots:
    void processMsg(std::vector<char> &data, int size, ushort portFrom);
    void slotCliConnected(quint16 port);
    void slotCliDisconnected(quint16 port);
    void showServPort(quint16 port);
    void printParsingResults(MessageParsingResult info);
private slots:
    void slotStartServer();
    void slotStopServer();
    void clearOutput();
private:
    Restarter*  _restarter;
    using TcpPort = ushort;
    using ShPtrParser
    = std::shared_ptr<ParsersManager<HeaderDescription<DataHeader>>>;
    std::map<TcpPort, ShPtrParser> _parsers{};
    // GUI:
    QTableWidget* _tableStatistics;
    QTextEdit*  _teStatistics;
    QTextEdit*  _teErrors;
    QTime*      _time;
    QLineEdit*  _lePort;
    QLineEdit*  _leRestartValue;
    QLabel*     _lbCurrentPort;
    QPushButton* _pbStart;
    QPushButton* _pbConnectionStatus;
    QPushButton* _pbClearOutput;
    QPalette    *_green0, *_red0, *_green1,
                *_red1, *_gray0, *_yellow0;
    // ---------------------------------------
    // TODO: Подумать над названием метода. Также: внутри метода
    // используется инф. о номере ряда, в который добавляются
    // данные. Это нехорошо.
    void addDataItemToRow(int column, const QVariant &data);

    void clearLabels(ClearLabelsPolicy fl = ClearLabelsPolicy::All);
    void printTimeAndSizeInfo(int msgSize);
    void printJsonObjAmount(ulong size);
    ShPtrParser getParser(TcpPort port);
//    void    updateGui();
};

#endif // WIDGET_H

