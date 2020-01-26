#include "tcpserver.h"
#include <QTcpServer>
#include <QDataStream>
#include <QDebug>

TcpServer::TcpServer(ushort connectionLimit)
    : _connectionLimit{connectionLimit}
{
    // На стороне сервера наблюдались пакеты, максимальный размер которых
    // был равен 3143184 (обмен через localhost),
    // был равен 4606400 (обмен по сети)
    _buff.reserve(6800100);
    _server = new QTcpServer(this);
    connect(_server, SIGNAL(newConnection()),   this, SLOT(slotNewConnection()) );
}

TcpServer::~TcpServer()
{
    close(); // Без этого метода тоже работает
    delete _server;
}

void TcpServer::start(ushort port)
{
    restart(port);
}

void TcpServer::restart(ushort port)
{
    close();
    open(port);
}

void TcpServer::close()
{   // Закрываем все входящие соединения:
    for (auto sock : _server->findChildren<QTcpSocket* >() )
        sock->disconnectFromHost();

    _server->close();
}

void TcpServer::open(quint16 port)
{
    qDebug() << "\n----- SERVER STARTED -----\n Port: " << port;
    bool isPortFree = _server->listen(QHostAddress::Any, port); // Разрешаем соединение с любого IP
    if (isPortFree == false)
    {
        if (_server->serverError() == QAbstractSocket::AddressInUseError)
        {
            qDebug() << "This port is already in use! Can't start the server!";
            close();
            emit portIsBusy();
        }
        else
            qDebug() << "Strange server behavior";
    }
    else
        emit listenPort(_server->serverPort());
}

void TcpServer::slotNewConnection()
{
    QTcpSocket* socket = _server->nextPendingConnection();
    if (isConnLimitOver())
    {
        delete socket; // Работает корректно
        return;
    }
    // Раскомментировать для отладки поведения сокетов:
//    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
//            this,     SLOT(slotCliStateChanged(QAbstractSocket::SocketState)) );
    connect(socket, SIGNAL(readyRead()),        this, SLOT(slotRead()) );
    connect(socket, SIGNAL(disconnected()),     this, SLOT(deleteSocket()) );

    emit clientConnected(socket->peerPort());
//    auto sz = _server->findChildren<QTcpSocket* >().size();
//    qDebug() << "Socket's amount WITH new connection: " << sz;
}


void TcpServer::slotRead()
{
    auto cliSocket = dynamic_cast<QTcpSocket*>(this->sender());
    QDataStream in(cliSocket);
    in.setVersion(QDataStream::Qt_5_3);
    // Явно уменьшаю len до размера int. Проблем не возникает:
    int len = static_cast<int>(cliSocket->bytesAvailable());
    // TODO: Заменить buff на std::string максимального размера (+-):
    in.readRawData(_buff.data(), len);
    emit haveData(_buff, len, cliSocket->peerPort());
}

void TcpServer::sendToClient(const QByteArray& ba)
{
    // Метод раздает идентичную информацию всем клиентам.
    // TODO: В будущем: реализовать выборочную передачу.
    for (auto sock : _server->findChildren<QTcpSocket* >() )
        sock->write(ba);
}

void TcpServer::slotCliStateChanged(QAbstractSocket::SocketState state) const
{
    qDebug() << "===> TCP-client state changed: " << state;
}

void TcpServer::deleteSocket()
{
    auto cliSocket = dynamic_cast<QTcpSocket*>(this->sender());
    if (cliSocket == nullptr)
        return;

    emit clientDisconnected(cliSocket->peerPort());
    // Удялять сокет следует через метод deleteLater().
    // delete cliSocket; - здесь так делать нельзя, программа будет падать
    cliSocket->deleteLater();
//    auto sz = _server->findChildren<QTcpSocket* >().size();
//    qDebug() << "Socket's amount after delete: " << sz;
}

bool TcpServer::isConnLimitOver() const
{
    if (_server->findChildren<QTcpSocket* >().size() > _connectionLimit)
        return true;
    else
        return false;
}

