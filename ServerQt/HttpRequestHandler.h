#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QUrlQuery>
#include <nlohmann/json.hpp>
#include <memory>

class HttpRequestHandler : public QObject
{
    Q_OBJECT

public:
    explicit HttpRequestHandler(const QString& basePath, QObject* parent = nullptr);

public slots:
    void handleRequest();

private:
    QString currentPath;
    const QString basePath;

    void processGetRequest(const QString& request);
    void sendResponse(QTcpSocket* socket, const QString& status,
        const QByteArray& content = QByteArray(),
        const QString& contentType = "text/plain") const;
    void sendJsonResponse(QTcpSocket* socket, const nlohmann::json& json) const;
    void handlePathChange(const QUrlQuery& query);
};