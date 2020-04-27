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
#include <memory>
#include "parsers/abstractparsersignalsslots.h"
#include "dataheader.h"

namespace MySpace {
const int a = 4;
const char b = 'b';
}

enum class ClearLabelsPolicy
{
    All,
    ExceptFirst
};


template<typename S, typename PFamily>
class ParsersManager;
class AbstractParser;
class AbstractP;
class DataHandler;
class TcpServer;
class QTime;
class Widget :
        public QWidget,
        public std::enable_shared_from_this<Widget>
{
    Q_OBJECT
public:
    Widget();
    ~Widget() override;
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
//    std::unique_ptr<Restarter> _restarter;
    std::unique_ptr<TcpServer> _server;

    using TcpPort = ushort;
//    using Header = DataHeader;
    using Header = EmptyHeader;
//    using PFamily = AbstractParser;
    using PFamily = AbstractP;
    using ShPtrParser = std::shared_ptr<ParsersManager<Header, PFamily>>;

    std::map<TcpPort, ShPtrParser> _parsers{};

    std::unique_ptr<DataHandler> _dataHandler;

    // GUI:
    QTableWidget* _tableStatistics;
    QTextEdit*  _teStatistics;
    QTextEdit*  _teErrors;
    std::unique_ptr<QTime> _time;
    QLineEdit*  _lePort;
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
    void showEvent(QShowEvent* /*event*/) override;
};

#endif // WIDGET_H

