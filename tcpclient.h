#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QThread>
#include <QTcpSocket>


class TcpClient : public QThread
{
    Q_OBJECT
public:
    explicit TcpClient(qintptr tid, QObject *parent = nullptr);
    ~TcpClient();
    void run();
    void targetGetDialogResponse();

signals:
    void error(QTcpSocket::SocketError socketError);

public slots:
    void on_ClientConnected();
    void on_ClientDisconnected();
    void on_ClientBytesWritten(qint64 bytesWritten);
    void on_ClientReadyRead();

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
};

#endif // TCPCLIENT_H
