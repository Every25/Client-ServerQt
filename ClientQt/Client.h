#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QNetworkReply>

class QTextEdit;

class Client : public QObject
{
    Q_OBJECT

public:
    Client(QObject* parent = nullptr);
    ~Client();

    void sendRequest(const QUrl& url);

signals:
    void dataReceived(const QStringList& items);
    void errorOccurred(const QString& errorString);

private:
    QNetworkAccessManager* manager;
    //QTcpSocket* socket;

public slots:
    void onFinished(QNetworkReply* reply);
};

