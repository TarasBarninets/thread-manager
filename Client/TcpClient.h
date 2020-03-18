#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

class TcpClient : public QObject
{
    Q_OBJECT

    public:
        TcpClient(QObject* parent);
        void requestToServer(const char* request);

    signals:
        void responseFromServer(QString message);

    private slots:
        void connectToServer();
        void serverResponse();

    private:
        QTcpSocket* mTcpSocket;
};

#endif // TCPCLIENT_H
