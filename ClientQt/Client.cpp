#include "Client.h"

#include <nlohmann/json.hpp>
#include <QString>
#include <QByteArray>
#include <string>

using json = nlohmann::json;

Client::Client(QObject* parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &Client::onFinished);
}

Client::~Client()
{}

void Client::sendRequest(const QUrl& url) {
    QNetworkRequest request(url);
    manager->get(request);
}

void Client::onFinished(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    std::string responseStr = std::string(responseData);
    QStringList items;

    try {
        std::string json_str = R"({
        "name": "Ivan",
        "age": 30,
        "skills": ["C++", "Python", "JavaScript"]
    })";
        // Парсим JSON с помощью nlohmann
        json j = json::parse(responseStr);

        if (j["files"].is_array()) {
            for (const auto& element : j["files"]) {
                if (element.is_string()) {
                    QString item = QString::fromStdString(element.get<std::string>());
                    items << item;
                }
            }
        }
        else {
            emit errorOccurred(QStringLiteral(u"Ответ не является массивом JSON"));
            reply->deleteLater();
            return;
        }
    }
    catch (const std::exception& e) {
        emit errorOccurred(QString(QStringLiteral(u"Ошибка парсинга JSON: %1")).arg(e.what()));
        reply->deleteLater();
        return;
    }

    emit dataReceived(items);
    reply->deleteLater();
}


