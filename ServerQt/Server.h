#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QDateTime> 
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <nlohmann/json.hpp>


class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject* parent = 0);

    void incomingConnection(qintptr handle);

    QString getDirectoryListing(const QString& path);

signals:

public slots:
    void onReadyRead();
    void onDisconnected();
};