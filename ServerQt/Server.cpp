#include "Server.h"
#include "HttpRequestHandler.h"
#include <QDebug>

Server::Server(QObject* parent) :
    QTcpServer(parent),
    requestHandler(new HttpRequestHandler(basePath, this))
{
    if (listen(QHostAddress::Any, 8080)) {
        qDebug() << "Server listening on port 8080";
    }
    else {
        qCritical() << "Failed to start server:" << errorString();
    }
}

Server::~Server()
{
}

void Server::incomingConnection(qintptr handle)
{
    QTcpSocket* clientSocket = new QTcpSocket(this);
    if (!clientSocket->setSocketDescriptor(handle)) {
        qWarning() << "Failed to set socket descriptor:" << clientSocket->errorString();
        delete clientSocket;
        return;
    }

    connect(clientSocket, &QTcpSocket::readyRead, requestHandler, &HttpRequestHandler::handleRequest);
    connect(clientSocket, &QTcpSocket::disconnected, this, &Server::onClientDisconnected);
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);

    qDebug() << "New connection from:" << clientSocket->peerAddress().toString();
}

void Server::onClientDisconnected()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        qDebug() << "Client disconnected:" << clientSocket->peerAddress().toString();
    }
}