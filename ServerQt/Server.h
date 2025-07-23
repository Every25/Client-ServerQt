#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QDateTime> 
#include <QFile>

class Server :public QTcpServer
{
	Q_OBJECT
public:
	explicit Server(QObject *parent = 0);

	void incomingConnection(qintptr handle);

signals:

public slots:
	void onReadyRead();
	void onDisconnected();
};