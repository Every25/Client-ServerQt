#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QDateTime> 
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QUrlQuery>
#include <nlohmann/json.hpp>


class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject* parent = 0);

    void incomingConnection(qintptr handle);

    QString getDirectoryListing(const QString& path);

private:
    QString currentPath; // Текущий путь
    const QString basePath = "./files"; // Базовая директория

public slots:
    void onReadyRead();
    void onDisconnected();
};