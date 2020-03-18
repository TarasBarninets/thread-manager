#include "TcpServer.h"

TcpServer::TcpServer(QObject *parent) : QObject(parent)
{
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection, this, &TcpServer::clientConnect);

    if(mTcpServer->listen(QHostAddress::Any, 6464))
    {
        qDebug() << "Server is started";
    } else
    {
        qDebug() << "Server is not started";
    }
}

void TcpServer::informClient(int fileId, QString path)
{
    auto it = mFileIdToClientSocket.find(fileId);
    if(it != mFileIdToClientSocket.end())
    {
        QTcpSocket* clientSocket = it.value();
        clientSocket->write(path.toStdString().c_str());
    }
}

void TcpServer::clientConnect()
{
    QTcpSocket* clientSocket = mTcpServer->nextPendingConnection();
    emit requestFromClient("Client connected!");

    clientSocket->write("Connected to server!");
    connect(clientSocket, &QTcpSocket::readyRead, this, &TcpServer::acceptRequest);
    connect(clientSocket, &QTcpSocket::disconnected, this, &TcpServer::clientDisconect);
}

void TcpServer::acceptRequest()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    QString& buffer = mClientSocketToBuffer[clientSocket];
    buffer.append(clientSocket->readAll());

    qDebug() << "Get from client - " << buffer;

    const QString firstWord = "request";
    const QString secondWord = "file";
    QString isNewLineSymbols;

    if(buffer.size() > 14)
    {
        isNewLineSymbols = buffer.right(1);
    }

    if(isNewLineSymbols == ";")
    {
        qDebug() << "Found new line - ; in mClientBuferrData";
        QTime time = QTime::currentTime();
        QString historyMessage = time.toString() + " Client has sent - " + buffer;
        qDebug() << historyMessage;
        emit requestFromClient(historyMessage);

        QStringList splittedRequest = buffer.split(" ");
        QString thirdPart = splittedRequest.at(2);
        int id = thirdPart.left(thirdPart.length()-1).toInt();

        qDebug() << "first - " << splittedRequest.at(0);
        qDebug() << "second - " << splittedRequest.at(1);
        qDebug() << "third - " << splittedRequest.at(2);
        qDebug() << "File id - " << id;

        mFileIdToClientSocket[id] = clientSocket;
        buffer.clear();

        emit requestFile(id);

        return;
    }
}

void TcpServer::clientDisconect()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    clientSocket->close();
    qDebug() << "Client disconnected";
}
