#include "HttpRequestHandler.h"
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QUrlQuery>
#include <QDebug>

HttpRequestHandler::HttpRequestHandler(const QString& basePath, QObject* parent)
    : QObject(parent), currentPath(basePath), basePath(basePath)
{
    QDir dir(basePath);
    if (!dir.exists()) {
        dir.mkpath(".");
        qInfo() << "Created base directory:" << basePath;
    }
}

//QString HttpRequestHandler::getIcon(const QString& thumbnailsTocation, const QString& iconName) const
//{
//    QString iconPath = QString("./icons/%1.svg").arg(iconName);
//    QFile iconFile(iconPath);
//
//    if (!iconFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        qInfo() << "Failed to open icon file:" << iconPath;
//        iconPath = QString("./icons/unknown.svg");
//        iconFile.setFileName(iconPath);
//        if (!iconFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
//            qCritical() << "Failed to open unknown icon file:" << iconPath;
//            return QString();
//        }
//    }
//
//    return QString::fromUtf8(iconFile.readAll());
//}


void HttpRequestHandler::handleRequest()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket || !socket->bytesAvailable()) {
        return;
    }

    try {
        QString request = QString::fromUtf8(socket->readAll());
        qDebug() << "Received request:" << request.left(200);

        if (request.startsWith("GET")) {
            processGetRequest(request);
            QFileInfo fileInfo(currentPath);

            if (!fileInfo.exists()) {
                sendResponse(socket, "404 Not Found", "Resource not found");
                return;
            }

            if (fileInfo.isDir()) {
                QDir librariesDir(currentPath);
                QStringList jsonFiles = librariesDir.entryList(QStringList() << "*.json", QDir::Files);

                if (!jsonFiles.isEmpty()) {
                    QString jsonFilePath = librariesDir.filePath(jsonFiles.first());
                    QFile jsonFile(jsonFilePath);

                    if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QByteArray jsonContent = jsonFile.readAll();
                        sendResponse(socket, "200 OK", jsonContent, "application/json");
                        jsonFile.close();
                        return;
                    }
                    else {
                        qWarning() << "Failed to open JSON file:" << jsonFilePath;
                    }
                }
                else {
                    qWarning() << "No JSON files found in folder";
                }
            }
        }
    }
    catch (const std::exception& e) {
        qCritical() << "Error processing request:" << e.what();
        sendResponse(socket, "500 Internal Server Error", "Server error occurred");
    }
}

void HttpRequestHandler::processGetRequest(const QString& request)
{
    int start = request.indexOf("GET /") + 5;
    int end = request.indexOf(" HTTP/", start);

    if (end > start) {
        QString requestedPath = request.mid(start, end - start);
        int paramStart = requestedPath.indexOf('?');

        if (paramStart != -1) {
            QString params = requestedPath.mid(paramStart + 1);
            requestedPath = requestedPath.left(paramStart);
            handlePathChange(QUrlQuery(params));
        }
    }
}

void HttpRequestHandler::handlePathChange(const QUrlQuery& query)
{
    if (query.hasQueryItem("path")) {
        QString folderName = query.queryItemValue("path");
        if (folderName == "") {
            folderName = basePath;
        }
        QString newPath = "./" + folderName;

        if (QDir(newPath).exists()) {
            currentPath = newPath;
        }
    }
}

void HttpRequestHandler::sendResponse(QTcpSocket* socket, const QString& status,
    const QByteArray& content, const QString& contentType) const
{
    if (!socket) return;

    QString response = QString("HTTP/1.1 %1\r\n"
        "Content-Type: %2\r\n"
        "Content-Length: %3\r\n"
        "\r\n")
        .arg(status)
        .arg(contentType)
        .arg(content.size());

    socket->write(response.toUtf8());
    if (!content.isEmpty()) {
        socket->write(content);
    }
    socket->disconnectFromHost();
}