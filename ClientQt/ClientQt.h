#pragma once

#include <QtWidgets/QMainWindow>

#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QNetworkReply>

class QTextEdit;

class ClientQt : public QMainWindow
{
    Q_OBJECT

public:
    ClientQt(QWidget *parent = nullptr);
    ~ClientQt();

private:
    QTextEdit* infoTextBrowser;
    QTcpSocket* socket;

public slots:
    void onResponse(QNetworkReply* reply);
    void onConnectButton();
    //void onGetButton();
};

