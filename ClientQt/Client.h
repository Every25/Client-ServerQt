#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QString>


class QTextEdit;

class Client : public QObject
{
    Q_OBJECT

public:
    Client(QObject* parent = nullptr);
    ~Client();

    void sendRequest(QUrl url);
    QString parameter;

signals:
    void dataReceived(const QStringList& items);
    void errorOccurred(const QString& errorString);

private:
    QNetworkAccessManager* manager;

public slots:
    void onFinished(QNetworkReply* reply);
};

