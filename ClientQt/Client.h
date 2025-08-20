#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QString>
#include <QStandardItemModel>
#include <nlohmann/json.hpp>

class QTextEdit;

class Client : public QObject
{
    Q_OBJECT

public:
    Client(QObject* parent = nullptr);
    ~Client();

    void sendRequest();
    QUrl url;
    QString currentPath = "/home";
    QUrlQuery query;

signals:
    void dataReceived(const nlohmann::json& jsonData);
    void errorOccurred(const QString& errorString);

private:
    QNetworkAccessManager* manager;

public slots:
    void onFinished(QNetworkReply* reply);
};

