#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include <QTcpSocket>
#include <memory>
// Класс реализует tcp-сервер. Класс используется следующим образом:
// сначала вызывается конструктор, затем - метод restart() или метод start().
// Периодический вызов метода restart() может быть использован для
// перезапуска сервера с новыми параметрами.
// Класс поддерживает управление несколькими входящими соединениями.

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
    void haveData(std::vector<char>& data, ushort portFrom);
    void portIsBusy();
    void listenPort(ushort localPort);
private slots:
    void slotNewConnection();
    // Слот используется только для отладки:
    void slotCliStateChanged(QAbstractSocket::SocketState state) const;
    void slotRead();
    void deleteSocket();
private:
    std::unique_ptr<QTcpServer> _server;
    std::vector<char> _buff;
    const ushort _connectionLimit;
    bool isConnLimitOver() const;
    void open(quint16 port);
};

#endif  // TCP_SERVER_H

