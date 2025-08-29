#include "Client.h"

#include <QByteArray>

using json = nlohmann::json;

Client::Client(QObject* parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &Client::onFinished);
}

Client::~Client()
{}

void Client::sendRequest(QString path) {

    if (!path.isEmpty()) {
        query.addQueryItem("path", path);
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
    QStandardItem* node = new QStandardItem();
    json j;
    try {
        // Парсинг JSON с помощью nlohmann
        j = json::parse(responseStr);
    }
    catch (const std::exception& e) {
        emit errorOccurred(QString(QStringLiteral(u"Ошибка парсинга JSON: %1")).arg(e.what()));
        reply->deleteLater();
        delete node;
        return;
    }
    QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
    if (contentType.startsWith("application/json")) {
            emit jsonReceived(j);
    }
    else if (contentType.startsWith("icon/")) {
        emit iconReceived(j);
    }
    else {
        emit errorOccurred(QString(QStringLiteral(u"Неизвестный тип данных")));
        reply->deleteLater();
    }
    reply->deleteLater();
}
