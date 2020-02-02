#include "restarter.h"
#include "widget.h"

Restarter::Restarter(std::weak_ptr<Widget> w)
    : _widget(w)
    , _port(0)
    , mode(TcpServerMode::ChangeBusyPort)
{
    _server = std::make_unique<TcpServer>(3);
    _timer = std::make_unique<QTimer>();

    connect(_timer.get(), &QTimer::timeout,  this, &Restarter::restartServer);
    // Данный коннект нужно расконнектить для работы режима one_attempt.
    // Эта задача не решена. Решить в будущем:
    connect(_server.get(), &TcpServer::portIsBusy,
            this, &Restarter::restartServer);
    connect(_server.get(), &TcpServer::haveData,
            _widget.lock().get(), &Widget::processMsg);

    connect(_server.get(), &TcpServer::listenPort,
            _widget.lock().get(), &Widget::showServPort);
    connect(_server.get(), &TcpServer::clientConnected,
            _widget.lock().get(), &Widget::slotCliConnected);
    connect(_server.get(), &TcpServer::clientDisconnected,
            _widget.lock().get(), &Widget::slotCliDisconnected);
}


Restarter::~Restarter()
{
    // Дописать delete для всех объектов:
    _server->disconnect();
    qDebug() << "Restarter::~dtor() called";
}


// Warn: Переименовать метод (init или др., текущее название неудачно)
void Restarter::restart(quint16 port, TcpServerMode mode, quint32 rstVal)
{
    _restartVal = rstVal;
    _port = port;
    setServerMode(mode); // Подход устарел. Режимы в этом проекте не нужны.

    _server->restart(port);
}


void Restarter::restartServer()
{
//    if (port == 0)
//    {
//        quint16 res = rand.operator ()();
//        res = res % nPorts; // Значения - [0; 23]
//        port = res + this->port;
//        qDebug() << "---------------------------------------------------------------";
//        qDebug() << "Restarter::restartServer() with port " << port;
//    }
    _server->restart(_port);
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
        _timer->start(1000 * _restartVal); // 3600
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
        _timer->stop();
        _server->close();
        return;
    }
}


void Restarter::servSendToClient(const QByteArray &ba)
{
    _server->sendToClient(ba);
}


