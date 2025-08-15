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

void HttpRequestHandler::handleRequest()
{
    std::vector<std::string> files;
    std::vector<std::string> folders;
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

            if (fileInfo.isDir()) {
                nlohmann::json jsonResponse;
                jsonResponse["files"] = nlohmann::json::array();

                QDir dir(currentPath);
                QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::Name);
                for (const QFileInfo& fileInfo : list) {
                    if (fileInfo.fileName().contains(".")) {
                        files.push_back(fileInfo.fileName().toStdString());
                    }
                    else {
                        folders.push_back(fileInfo.fileName().toStdString());
                    }
                }
                sort(folders.begin(), folders.end());
                sort(files.begin(), files.end());

                for (const auto& folder : folders) {
                    jsonResponse["files"].push_back(folder);
                }
                for (const auto& file : files) {
                    jsonResponse["files"].push_back(file);
                }

                sendJsonResponse(socket, jsonResponse);
            }

            else {
                sendResponse(socket, "404 Not Found", "Resource not found");
            }
        }
        else {
            sendResponse(socket, "405 Method Not Allowed", "Only GET method is supported");
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

void HttpRequestHandler::sendJsonResponse(QTcpSocket* socket, const nlohmann::json& json) const
{
    std::string jsonStr = json.dump();
    sendResponse(socket, "200 OK",
        QByteArray::fromStdString(jsonStr),
        "application/json");
}

