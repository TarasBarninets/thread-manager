#include "TcpClient.h"

TcpClient::TcpClient(QObject *parent): QObject(parent), mTcpSocket(new QTcpSocket(this))
{
    mTcpSocket->connectToHost("localhost", 6464);

    connect(mTcpSocket, &QTcpSocket::connected, this, &TcpClient::connectToServer);
    connect(mTcpSocket, &QTcpSocket::readyRead, this, &TcpClient::serverResponse);
}

void TcpClient::connectToServer()
{
    qDebug() << "Connected to server!";
}

void TcpClient::serverResponse()
{
    QString response(mTcpSocket->readAll());
    emit responseFromServer(response);
}

void TcpClient::requestToServer(const char* request)
{
    mTcpSocket->write(request);
    qDebug() << "Sent to server : " << request;
}
