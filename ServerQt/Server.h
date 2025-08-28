#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QDir>
#include <memory>

class HttpRequestHandler;

class Server : public QTcpServer
{
    Q_OBJECT

public:
    explicit Server(QObject* parent = nullptr);
    ~Server() override;

protected:
    void incomingConnection(qintptr handle) override;

private:
    const QString basePath = "./Libraries";
    HttpRequestHandler* requestHandler;

private slots:
    void onClientDisconnected();
};