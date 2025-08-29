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

    void sendRequest(QString path);
    QUrl url;
    QUrlQuery query;

signals:
    void jsonReceived(const nlohmann::json& jsonData);
    void iconReceived(const nlohmann::json& jsonObj);
    void errorOccurred(const QString& errorString);

private:
    QNetworkAccessManager* manager;

public slots:
    void onFinished(QNetworkReply* reply);
};

