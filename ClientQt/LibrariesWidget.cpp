#include "LibrariesWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QMessageBox>

QString ip = "127.0.0.1";
//QString ip = "10.0.1.118";
int port = 8080;

LibrariesWidget::LibrariesWidget(QWidget* parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);
    auto buttonLayout = new QHBoxLayout(this);

    listView = new QListView();
    model = new QStringListModel();
    listView->setModel(model);
    mainLayout->addWidget(listView);
    listView->installEventFilter(this);

    backButton = new QPushButton(QStringLiteral(u"Назад"), this);
    forwardButton = new QPushButton(QStringLiteral(u"Вперед"), this);
    homeButton = new QPushButton(QStringLiteral(u"На главную"), this);
    backButton->setVisible(false);
    forwardButton->setVisible(false);
    homeButton->setVisible(false);
    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(forwardButton);
    buttonLayout->addWidget(homeButton);
    

    mainLayout->addLayout(buttonLayout);
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
    client->key = "name";
    client->parameter = selectedItem;

    client->sendRequest();

    backButton->setVisible(true);
    forwardButton->setVisible(true);
    homeButton->setVisible(true);
}

void LibrariesWidget::backButtonClicked()
{
    client->key = "level";
    client->parameter = "back";
    client->sendRequest();
}

void LibrariesWidget::forwardButtonClicked()
{
    client->key = "level";
    client->parameter = "forward";
    client->sendRequest();
}

void LibrariesWidget::homeButtonClicked()
{
    client->key = "level";
    client->parameter = "home";
    client->sendRequest();
}