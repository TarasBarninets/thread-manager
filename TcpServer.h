#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QTime>
#include <QTextEdit>

class TcpServer : public QObject
{
    Q_OBJECT

public:
    TcpServer(QObject *parent);
    void informClient(int fileId, QString path);

signals:
    void requestFile(int fileId);
    void requestHistoryMessage(QString message);

private slots:
    void clientConnect();
    void acceptRequest();
    void clientDisconect();

private:
    QTcpServer* mTcpServer;
    QMap <int, QTcpSocket*> mFileIdToClientSocket;
    QMap <QTcpSocket*, QString> mClientSocketToBuffer;
};

#endif // TCPSERVER_H
