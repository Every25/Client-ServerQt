#include "ClientQt.h"


#include <QVBoxLayout>
#include <QPushButton>
#include <QTextBrowser>


ClientQt::ClientQt(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget* centralWidget = new QWidget;
    setCentralWidget(centralWidget);
   
    socket = new QTcpSocket();
    
    QVBoxLayout* mainLayout = new QVBoxLayout;
    centralWidget->setLayout(mainLayout);

    QPushButton* connectButton = new QPushButton("connect");
    connect(connectButton, &QPushButton::clicked, this, &ClientQt::onConnectButton);
    mainLayout->addWidget(connectButton);

    infoTextBrowser = new QTextBrowser;
    infoTextBrowser->setFixedSize(400, 300);
    mainLayout->addWidget(infoTextBrowser);

    QPushButton* getButton = new QPushButton("get");
    //connect(getButton, &QPushButton::clicked, this, &ClientQt::onGetButton);
    mainLayout->addWidget(getButton);
}

ClientQt::~ClientQt()
{}

void ClientQt::onResponse(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        // Например, вывести в текстовое поле или консоль
        infoTextBrowser->setPlainText(QString::fromUtf8(data));
    }
    else {
        qDebug() << "Error:" << reply->errorString();
    }
}

void ClientQt::onConnectButton()
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);

    connect(manager, &QNetworkAccessManager::finished,
        this, &ClientQt::onResponse);

    //QUrl url("http://10.0.0.232:8080");//IP сервера тусур сеть
    QUrl url("http://127.0.0.1:8080");//свой IP
    QNetworkRequest request(url);

    manager->get(request);
}

//void ClientQt::onGetButton()
//{ 
//    QString request = "GET /test.txt HTTP/1.1\r\nHost: " + ipLineEdit->text() + "\r\n\r\n";
//    socket->write(request.toUtf8());
//    infoTextBrowser->append("Request sent...");
//    requestButton->setEnabled(false); // чтобы не отправлять повторно пока не получим ответ
//}
// else {
//     responseTextEdit->append("Not connected to server.");
//        }
//
//}


