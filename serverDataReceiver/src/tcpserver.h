#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include <QTcpSocket>

// Класс реализует tcp-сервер. Класс используется следующим образом:
// сначала вызывается конструктор, затем - метод restart(). Периодический
// вызов метода restart() осуществляет рестарт сервера с новыми параметрами.

// TODO: Дополнить позже:
// Класс поддерживает управление несколькими входящими соединениями.
// Входящий трафик распределяется между объектами-получателями.

class QTcpServer;
class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(ushort connectionLimit);
    ~TcpServer();
    void start(ushort port);
    void restart(ushort port); // == close() + open()
    void close();
    void sendToClient(const QByteArray& ba);
signals:
    void clientConnected(ushort remotePort);
    void clientDisconnected(ushort remotePort);
//    void haveData(char* buff, int len, ushort remotePort);
    void haveData(std::vector<char>& data, int len, ushort portFrom);
    void portIsBusy();
    void listenPort(ushort localPort);
private slots:
    void slotNewConnection();
    // Слот используется только для отладки:
    void slotCliStateChanged(QAbstractSocket::SocketState state) const;
    void slotRead();
    void deleteSocket();
private:
    QTcpServer*       _server;
    std::vector<char> _buff;
    const ushort _connectionLimit;
    bool isConnLimitOver() const;
    void open(quint16 port);
};

#endif  // TCP_SERVER_H

