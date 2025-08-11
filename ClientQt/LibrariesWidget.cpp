#include "LibrariesWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QString>

//QString ip = "127.0.0.1";
QString ip = "10.0.1.118";
int port = 8080;

LibrariesWidget::LibrariesWidget(QWidget* parent)
    : QWidget(parent)
{
    /*QWidget* centralWidget = new QWidget;
    setCentralWidget(centralWidget);*/

    auto layout = new QVBoxLayout(this);

    listView = new QListView();
    model = new QStringListModel();
    listView->setModel(model);
    layout->addWidget(listView);

    /*QStringList list;
    list << "Element 1" << "Element 2" << "Element 3" << "Element 4";
    model->setStringList(list);*/

    QPushButton* requestButton = new QPushButton(QStringLiteral(u"Запрос"), this);
    layout->addWidget(requestButton);

    setLayout(layout);
    
    // Создаем клиента
    client = new Client(this);

    connect(requestButton, &QPushButton::clicked, [=]() {
        client->sendRequest(QUrl("http://" + ip + ":" + QString::number(port)));
    });
    connect(client, &Client::dataReceived, this, &LibrariesWidget::updateList);
    connect(client, &Client::errorOccurred,this, &LibrariesWidget::handleError);
}

LibrariesWidget::~LibrariesWidget()
{
}

void LibrariesWidget::updateList(const QStringList& items) 
{
    model->setStringList(items);
}

void LibrariesWidget::handleError(const QString& errorString)
{
    qWarning() << "Ошибка при получении данных:" << errorString;
}
