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
    model = new QStandardItemModel(this);
    listView->setModel(model);
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listView->installEventFilter(this);

    refreshButton = new QPushButton(QIcon("icons/refresh.svg"), "", this);
    homeButton = new QPushButton(QIcon("icons/home.svg"), "", this);
    backButton = new QPushButton(QIcon("icons/back.svg"), "", this);
    forwardButton = new QPushButton(QIcon("icons/forward.svg"), "", this);
    homeButton->setEnabled(false);
    backButton->setEnabled(false);
    forwardButton->setEnabled(false);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(homeButton);
    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(forwardButton);
    

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(listView);
    setLayout(mainLayout);
    
    // Создаем клиента
    client = new Client(this);
    client->url = QUrl("http://" + ip + ":" + QString::number(port));

    backStack = new QStack<QString>();
    forwardStack = new QStack<QString>();

    connect(listView, &QListView::doubleClicked, this, &LibrariesWidget::RequestWithSelectedItem);
    connect(client, &Client::dataReceived, this, &LibrariesWidget::updateList);
    connect(client, &Client::errorOccurred,this, &LibrariesWidget::handleError);

    connect(refreshButton, &QPushButton::clicked, this, &LibrariesWidget::refreshButtonClicked);
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
    model->clear();

    for (const QString& item : items) {
        QStandardItem* standardItem = new QStandardItem();
        standardItem->setText(item);

        if (!item.contains('.')) {
            standardItem->setIcon(QIcon::fromTheme("folder"));
        }
        else {
            standardItem->setIcon(QIcon::fromTheme("text-x-generic"));
        }

        model->appendRow(standardItem);
    }
}

void LibrariesWidget::handleError(const QString& errorString)
{
    QMessageBox::warning(this, QStringLiteral(u"Ошибка при получении данных: "), errorString);
}

void LibrariesWidget::RequestWithSelectedItem()
{
    QModelIndex index = listView->currentIndex();
    QString selectedItem = model->data(index, Qt::DisplayRole).toString();
    if (!index.isValid() || selectedItem.contains('.')) {
        return;
    }

    backStack->push(client->currentPath);
    forwardStack->clear();

    client->currentPath += "/" + selectedItem;

    client->sendRequest();

    UpdateButtons();
}

void LibrariesWidget::refreshButtonClicked()
{
    client->sendRequest();
}

void LibrariesWidget::homeButtonClicked()
{
    backStack->clear();
    forwardStack->clear();

    client->currentPath = "/home";
    client->sendRequest();

    UpdateButtons();
}

void LibrariesWidget::backButtonClicked()
{
    if (!backStack->isEmpty()) {
        forwardStack->push(client->currentPath);
        forwardButton->setEnabled(true);

        client->currentPath = backStack->pop();

        client->sendRequest();
        UpdateButtons();

        // Если достигли корня, отключаем кнопку назад
        if (client->currentPath == "/home") {
            backButton->setEnabled(false);
        }
        else {
            backButton->setEnabled(true);
        }
    }
}

void LibrariesWidget::forwardButtonClicked()
{
    if (!forwardStack->isEmpty()) {
        backStack->push(client->currentPath);
        backButton->setEnabled(true);

        client->currentPath = forwardStack->pop();

        client->sendRequest();
        UpdateButtons();

        // Если стек вперёд пуст, отключаем кнопку "Вперёд"
        if (forwardStack->isEmpty()) {
            forwardButton->setEnabled(false);
        }
    }
}

void LibrariesWidget::UpdateButtons()
{
    backButton->setEnabled(!backStack->isEmpty());
    forwardButton->setEnabled(!forwardStack->isEmpty());
    homeButton->setEnabled(client->currentPath != "/home");
}