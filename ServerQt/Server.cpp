#include "Server.h"

Server::Server(QObject* parent) :
    QTcpServer(parent), currentPath("./files")
{
    if (listen(QHostAddress::Any, 8080)) {
        qDebug() << "Listening...";
    }
    else {
        qDebug() << "Error while starting: " << errorString();
    }
}

void Server::incomingConnection(qintptr handle)
{
    QTcpSocket* socket = new QTcpSocket();
    socket->setSocketDescriptor(handle);

    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

QString Server::getDirectoryListing(const QString& path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return "{\"error\":\"Directory not found\"}";
    }

    nlohmann::json j;
    j["files"] = nlohmann::json::array();

    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::Name);
    for (const QFileInfo& fileInfo : list) {
        j["files"].push_back(fileInfo.fileName().toStdString());
    }

    return QString::fromStdString(j.dump());
}

void Server::onReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket)
        return;

    QByteArray requestData = socket->readAll();
    qDebug() << "Received request:" << requestData;

    QString request = QString(requestData);
    QString path = currentPath; // Используем текущий путь

    // Обработка GET запроса с параметрами
    if (request.startsWith("GET /")) {
        int start = request.indexOf("GET /") + 5;
        int end = request.indexOf(" HTTP/", start);
        if (end > start) {
            QString requestedPath = request.mid(start, end - start);

            // Проверяем, есть ли параметры в URL
            int paramStart = requestedPath.indexOf('?');
            if (paramStart != -1) {
                QString params = requestedPath.mid(paramStart + 1);
                requestedPath = requestedPath.left(paramStart);

                // Разбираем параметры
                QUrlQuery query(params);
                if (query.hasQueryItem("name")) {
                    QString folderName = query.queryItemValue("name");
                     // Добавляем новую папку к текущему пути
                    path = currentPath + "/" + folderName;
                    QDir dir(path);
                    if (dir.exists()) {
                        currentPath = path; // Обновляем текущий путь
                    }
                }
            }
            else {
                // Запрос без параметров - сброс к корневой папке
                currentPath = basePath;
                path = currentPath;
            }
        }
    }

    QFileInfo fileInfo(path);
    QString response;

    if (fileInfo.isDir()) {
        QString directoryContent = getDirectoryListing(path);
        response = "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: " + QString::number(directoryContent.size()) + "\r\n"
            "\r\n" + directoryContent;
    }
    else if (fileInfo.isFile()) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            response = "HTTP/1.1 500 Internal Server Error\r\n\r\nFailed to open file.";
        }
        else {
            QByteArray fileContent = file.readAll();
            file.close();
            response = "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/octet-stream\r\n"
                "Content-Length: " + QString::number(fileContent.size()) + "\r\n"
                "\r\n";
            socket->write(response.toLatin1());
            socket->write(fileContent);
            socket->disconnectFromHost();
            return;
        }
    }
    else {
        response = "HTTP/1.1 404 Not Found\r\n\r\nResource not found.";
    }

    socket->write(response.toLatin1());
    socket->disconnectFromHost();
}

void Server::onDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        qDebug() << "Client disconnected:" << socket->peerAddress().toString();
        socket->deleteLater();
    }
}