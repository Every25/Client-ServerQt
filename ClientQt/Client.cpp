#include "Client.h"

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
    QStandardItem* node = new QStandardItem();

    try {
        // Парсинг JSON с помощью nlohmann
        json j = json::parse(responseStr);
        emit dataReceived(j);
        //addJsonToModel(j, node);
    }
    catch (const std::exception& e) {
        emit errorOccurred(QString(QStringLiteral(u"Ошибка парсинга JSON: %1")).arg(e.what()));
        reply->deleteLater();
        delete node;
        return;
    }

    //emit dataReceived(node);
    reply->deleteLater();
}

//// Функция для добавления элементов из JSON
//void Client::addJsonToModel(const nlohmann::json& jsonObj, QStandardItem* parentItem)
//{
//    if (jsonObj.contains("files") && jsonObj["files"].is_array())
//    {
//        auto files = jsonObj["files"];
//        for (auto& file : files)
//        {
//            QStandardItem* name = new QStandardItem(file["name"].get<std::string>().c_str());
//            parentItem->appendRow(name);
//
//            /*QStandardItem* icon = new QStandardItem(file["icon"].get<std::string>().c_str());
//            parentItem->appendRow(icon);*/
//        }
//    }
//
//}
