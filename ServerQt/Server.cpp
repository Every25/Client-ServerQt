#include "Server.h"

Server::Server(QObject* parent) :
	QTcpServer(parent)
{
	if (listen(QHostAddress::Any, 8080)) {
		qDebug() << "Listening...";
	}
	else {
		qDebug() << "Error while starting: " << errorString();
	}
}

void Server::incomingConnection(qintptr handle)
{
	QTcpSocket* socket = new QTcpSocket();
	socket->setSocketDescriptor(handle);

	connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

void Server::onReadyRead()
{
	QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
	if (!socket)
		return;

	QByteArray requestData = socket->readAll();
	qDebug() << "Received request:" << requestData;

	QFile file("./files/test.txt");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Failed to open file";
		// Можно отправить ошибку клиенту
		QString response = "HTTP/1.1 500 Internal Server Error\r\n\r\nFailed to open file.";
		socket->write(response.toLatin1());
		socket->disconnectFromHost();
		return;
	}

	QByteArray fileContent = file.readAll();
	file.close();

	QString responseHeader = "HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: %1\r\n"
		"\r\n";

	QString response = responseHeader.arg(fileContent.size());
	socket->write(response.toLatin1());       // Отправляем заголовки
	socket->write(fileContent);               // Отправляем содержимое файла
	socket->disconnectFromHost();
}

void Server::onDisconnected()
{
	QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
	if (socket) {
		qDebug() << "Client disconnected:" << socket->peerAddress().toString();
		socket->deleteLater();
	}
}