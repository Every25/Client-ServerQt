#include "LibrariesWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QMessageBox>

//QString ip = "127.0.0.1";
QString ip = "10.0.1.118";
int port = 8080;

LibrariesWidget::LibrariesWidget(QWidget* parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);
    auto buttonLayout = new QHBoxLayout(this);

    listView = new QListView();
    model = new QStringListModel();
    listView->setModel(model);
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listView->installEventFilter(this);

    homeButton = new QPushButton(QIcon("icons/home.svg"), "", this);
    backButton = new QPushButton(QIcon("icons/back.svg"), "", this);
    forwardButton = new QPushButton(QIcon("icons/forward.svg"), "", this);
    homeButton->setEnabled(false);
    backButton->setEnabled(false);
    forwardButton->setEnabled(false);
    buttonLayout->addWidget(homeButton);
    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(forwardButton);
    

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(listView);
    setLayout(mainLayout);
    
    // Создаем клиента
    client = new Client(this);
    client->url = QUrl("http://" + ip + ":" + QString::number(port));

    connect(client, &Client::dataReceived, this, &LibrariesWidget::updateList);
    connect(client, &Client::errorOccurred,this, &LibrariesWidget::handleError);

    connect(backButton, &QPushButton::clicked, this, &LibrariesWidget::backButtonClicked);
    connect(forwardButton, &QPushButton::clicked, this, &LibrariesWidget::forwardButtonClicked);
    connect(homeButton, &QPushButton::clicked, this, &LibrariesWidget::homeButtonClicked);

    client->sendRequest();
}

LibrariesWidget::~LibrariesWidget()
{
}

//Обработка нажатия кнопок
bool LibrariesWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == listView && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            RequestWithSelectedItem();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void LibrariesWidget::updateList(const QStringList& items) 
{
    model->setStringList(items);
}

void LibrariesWidget::handleError(const QString& errorString)
{
    QMessageBox::warning(this, QStringLiteral(u"Ошибка при получении данных: "), errorString);
}

void LibrariesWidget::RequestWithSelectedItem()
{
    QModelIndex index = listView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QString selectedItem = model->data(index, Qt::DisplayRole).toString();
    client->currentPath += "/" + selectedItem;

    client->sendRequest();
    if (client->currentPath != "/home")
    {
        homeButton->setEnabled(true);
        backButton->setEnabled(true);
    }
    else
    {
        backButton->setEnabled(false);
    }
    //forwardButton->setEnabled(true); 
}

void LibrariesWidget::homeButtonClicked()
{
    client->currentPath = "/home";
    client->sendRequest();
    homeButton->setEnabled(false);
    backButton->setEnabled(false);
}

void LibrariesWidget::backButtonClicked()
{
    if (client->currentPath != "/") {
        int lastSlashIndex = client->currentPath.lastIndexOf('/', client->currentPath.length() - 2);
        if (lastSlashIndex == -1) {
            client->currentPath = "/";
        }
        else {
            client->currentPath = client->currentPath.left(lastSlashIndex);
        }
        client->sendRequest();
    }
    client->sendRequest();
}

void LibrariesWidget::forwardButtonClicked()
{
   //
}