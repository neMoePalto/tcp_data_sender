#ifndef RESTARTER_H
#define RESTARTER_H
#include <QObject>
#include <QTimer>
#include <QDebug>
#include <tr1/random>
#include "tcpserver.h"

// Класс управляет сервером в соответствии с выбранным режимом.
// Last refactoring: 2017.04.21

enum class TcpServerMode
{
    OneAttempt      = 0,
    ChangeBusyPort  = 1,
    RandPortRestart = 2
};


class Widget;
class Restarter : public QObject
{
    Q_OBJECT
public:
    explicit Restarter(Widget *w); // Warn: Разобраться с объектными иерархиями
    ~Restarter(); // f.e.
    void restart(quint16 port, TcpServerMode mode, quint32 rstVal = 0);
    void close();
    void servSendToClient(const QByteArray &ba);

private slots:
    // Внутри метода реализована обработка ситуации, когда port == 0.
    // Тем не менее, для первого запуска порт необходим! : - Cтранный подход. Разобраться. Переписать.
    void restartServer(quint16 port = 0);

private:
    Widget      *wgt;
    QTimer      *tmr;
    TcpServer*  _server;
    ushort      port;
    TcpServerMode   mode;
    int         restartVal;
    std::tr1::random_device rand;
    const uint nPorts = 9; //24

    void setServerMode(TcpServerMode mode); // Смысл и роль метода выражены неконкретно
};

#endif // RESTARTER_H

