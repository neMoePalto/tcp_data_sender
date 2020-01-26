#ifndef TCP_SMARTCLIENT_H
#define TCP_SMARTCLIENT_H
#include <QHostAddress>

// Класс реализует tcp-клиент с функцией автоматического периодического
// подключения к серверу:
class QTcpSocket;
class TcpSmartClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpSmartClient(QHostAddress serverIP, ushort port);
    ~TcpSmartClient();
    qint64 sendToServer(const char* data, qint64 size);
//    void sendToServer(const QByteArray& ba);
signals:
    void connected();
    void disconnected();
    void haveMsg(char*, int);
//    void haveMsg(QByteArray);
private slots:
    void slotError(const QAbstractSocket::SocketError);
    void slotRead();
    void connectToHost();
private:
    QTcpSocket* _cliSocket;
    const QHostAddress _serverIP;
    const ushort _port;
    char* _buff;
    // TODO: Сделать параметром конструктора:
    const int _reconnectInterv{1000}; // Msec
};

#endif // TCP_SMARTCLIENT_H

