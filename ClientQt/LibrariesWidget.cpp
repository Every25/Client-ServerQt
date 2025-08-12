#include "LibrariesWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QString>
#include <QMessageBox>

QString ip = "127.0.0.1";
//QString ip = "10.0.1.118";
int port = 8080;

LibrariesWidget::LibrariesWidget(QWidget* parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);

    listView = new QListView();
    model = new QStringListModel();
    listView->setModel(model);
    layout->addWidget(listView);
    listView->installEventFilter(this);

    setLayout(layout);
    
    // Создаем клиента
    client = new Client(this);

    connect(client, &Client::dataReceived, this, &LibrariesWidget::updateList);
    connect(client, &Client::errorOccurred,this, &LibrariesWidget::handleError);

    client->sendRequest(QUrl("http://" + ip + ":" + QString::number(port)));
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
    client->parameter = selectedItem;

    client->sendRequest(QUrl("http://" + ip + ":" + QString::number(port)));
}
