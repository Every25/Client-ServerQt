#include "Client.h"

#include <nlohmann/json.hpp>
#include <QByteArray>

using json = nlohmann::json;

Client::Client(QObject* parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &Client::onFinished);
}

Client::~Client()
{}

void Client::sendRequest() {

    if (!currentPath.isEmpty()) {
        query.addQueryItem("path", currentPath);
    }
    url.setQuery(query.toString());
    manager->get(QNetworkRequest(url));
    query.clear();
}

void Client::onFinished(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError ||
        (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).isValid() &&
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() >= 400)) {
        emit errorOccurred(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    std::string responseStr = std::string(responseData);
    QStringList items;

    try {
        // Парсинг JSON с помощью nlohmann
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
