#include "restarter.h"
#include "widget.h"

Restarter::Restarter(Widget *w)
    : wgt(w)
    , port(0)
    , mode(TcpServerMode::ChangeBusyPort)
{
    _server = new TcpServer(3);
    tmr     = new QTimer();

    connect(tmr,  SIGNAL(timeout()),                        this, SLOT(restartServer()) );
    // Данный коннект нужно расконнектить для работы режима one_attempt.
    // Эта задача не решена. Решить в будущем:
    connect(_server, SIGNAL(portIsBusy()),                  this, SLOT(restartServer()) );

    connect(_server, &TcpServer::haveData,  w, &Widget::processMsg);

    connect(_server, SIGNAL(listenPort(quint16)),           w, SLOT(showServPort(quint16)) );
    connect(_server, SIGNAL(clientConnected(quint16)),      w, SLOT(slotCliConnected(quint16)) );
    connect(_server, SIGNAL(clientDisconnected(quint16)),   w, SLOT(slotCliDisconnected(quint16)) );
}


Restarter::~Restarter()
{
    // Дописать delete для всех объектов:
    delete _server;
    qDebug() << "Restarter::~dtor() called";
}


// Warn: Переименовать метод (init или др., текущее название неудачно)
void Restarter::restart(quint16 port, TcpServerMode mode, quint32 rstVal)
{
    restartVal = rstVal;
    this->port = port;
    setServerMode(mode);

    restartServer(port);
}


void Restarter::restartServer(quint16 port)
{
    if (port == 0)
    {
        quint16 res = rand.operator ()();
        res = res % nPorts; // Значения - [0; 23]
        port = res + this->port;
        qDebug() << "---------------------------------------------------------------";
        qDebug() << "Restarter::restartServer() with port " << port;
    }
    _server->restart(port);
}

void Restarter::setServerMode(TcpServerMode mode)
{
    this->mode = mode;

    if (mode == TcpServerMode::OneAttempt)
    {
        // Режим не реализован. Здесь нужно производить disconnect (см. комментарии в конструкторе класса).
        // Проблема в том, что после этого нужно реализовать логику повторного коннекта.
        return;
    }

    if (mode == TcpServerMode::ChangeBusyPort)
        return;

    if (mode == TcpServerMode::RandPortRestart)
    {
        tmr->start(1000 * restartVal); // 3600
        return;
    }
}

void Restarter::close()
{
    if (mode == TcpServerMode::OneAttempt)
    {
        _server->close();
        return;
    }

    if (mode == TcpServerMode::ChangeBusyPort)
    {
        _server->close();
        return;
    }

    if (mode == TcpServerMode::RandPortRestart)
    {
        tmr->stop();
        _server->close();
        return;
    }
}


void Restarter::servSendToClient(const QByteArray &ba)
{
    _server->sendToClient(ba);
}


