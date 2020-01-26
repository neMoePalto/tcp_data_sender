#include "tcpsmartclient.h"
#include <QTimer>
#include <QTcpSocket>
#include <QDataStream>
#include <QDebug>
#include "netinet/in.h"

TcpSmartClient::TcpSmartClient(QHostAddress serverIP, ushort port)
    : _serverIP(serverIP)
    , _port(port)
{   // TODO: Разобраться с размером буфера для tcp. Текущее значение
    // подобрано опытным путем:
    _buff = new char[6800100];
    _cliSocket = new QTcpSocket(this);
//    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");

    connect(_cliSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,       SLOT(slotError(QAbstractSocket::SocketError)) ); //, Qt::DirectConnection);
    connect(_cliSocket, SIGNAL(connected()),    this, SIGNAL(connected()) ); //, Qt::DirectConnection);
    connect(_cliSocket, SIGNAL(disconnected()), this, SIGNAL(disconnected()) );
    connect(_cliSocket, SIGNAL(readyRead()),    this, SLOT(slotRead()) );

    connectToHost();
}

TcpSmartClient::~TcpSmartClient()
{
    qDebug() << "TcpSmartClient::~dtor()";
    delete _cliSocket;
    delete[] _buff;
}

void TcpSmartClient::connectToHost()
{
    _cliSocket->connectToHost(_serverIP, _port);
    qDebug() << "TcpSmartClient: try to connect with port" << _port;
}

qint64 TcpSmartClient::sendToServer(const char* data, qint64 size)
{
    qint64 res = _cliSocket->write(data, size);
    if (res != size)
    {
        qDebug() << "TcpSmartClient::sendToServer() : uncorrect data sending."
                    " Size (exit value) = " << res;
    }
    return res;
}


void TcpSmartClient::slotError(const QAbstractSocket::SocketError error)
{
    const QString methodName{"TcpSmartClient::slotError() : "};
    switch (error)
    {
    case QAbstractSocket::RemoteHostClosedError:
        qDebug() << methodName + "RemoteHostClosedError" << _port;
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() << methodName + "HostNotFoundError" << _port;
        break;
    case QAbstractSocket::ConnectionRefusedError:
//        qDebug() << methodName + "ConnectionRefusedError" << port;
        break;
    default:
//        str.append(tr("- The following error occurred: %1.").arg(socket->errorString()));
        qDebug() << methodName + _cliSocket->errorString() + " "
                    + _serverIP.toString() + ":"+ QString::number(_port);
        break;
    }

    // Метод не разрывает соединение немедленно. Это может иметь значение:
    _cliSocket->disconnectFromHost();
    QTimer::singleShot(_reconnectInterv, this, SLOT(connectToHost()) );
}

// TODO: Скорректировать метод по образцу TcpServer::slotRead()
void TcpSmartClient::slotRead()
{
    QDataStream in(_cliSocket);
    in.setVersion(QDataStream::Qt_5_3);
    int len = static_cast<int>(_cliSocket->bytesAvailable());

    in.readRawData(_buff, len);
    qDebug () << "TcpSmartClient::slotRead() : Received data size= " << len;
    emit haveMsg(_buff, len);
}

