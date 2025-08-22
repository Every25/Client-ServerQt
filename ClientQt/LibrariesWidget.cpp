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
    libraries = new QList<Library>;
    catalogs = new QList<Catalog>;

    treeView = new QTreeView(this);
    model = new QStandardItemModel(this);
    root = model->invisibleRootItem();
    treeView->setModel(model);
    treeView->setHeaderHidden(true);
    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    treeView->installEventFilter(this);

    //создание и добавление кнопок
    refreshButton = new QPushButton(QIcon("icons/refresh.svg"), "", this);
    buttonLayout->addWidget(refreshButton);
    

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(treeView);
    setLayout(mainLayout);
    
    // Создание клиента
    client = new Client(this);
    client->url = QUrl("http://" + ip + ":" + QString::number(port));

    //привязка сигналов
    connect(treeView, &QTreeView::doubleClicked, this, &LibrariesWidget::RequestWithSelectedItem);
    connect(treeView, &QTreeView::expanded, this, &LibrariesWidget::RequestWithSelectedItem);
    connect(client, &Client::dataReceived, this, &LibrariesWidget::updateTree);
    connect(client, &Client::errorOccurred,this, &LibrariesWidget::handleError);

    //привязка сигналов для кнопок
    connect(refreshButton, &QPushButton::clicked, this, &LibrariesWidget::refreshButtonClicked);

    //первое отправление запроса
    client->sendRequest();
}

LibrariesWidget::~LibrariesWidget()
{
   delete libraries;
   delete catalogs;
}

void LibrariesWidget::RequestWithSelectedItem(const QModelIndex& index)
{
    QString selectedItem = model->data(index, Qt::DisplayRole).toString();
    if (!index.isValid()) {
        return;
    }
    foreach(const auto& lib, *libraries) {
        if (lib.name == selectedItem) {
            QString fullPath = "/Libraries/" + lib.dir;
            client->currentPath = fullPath;
            client->sendRequest();
            currentLibrary = lib;
            return;
        }
    }
    foreach(const auto& cat, *catalogs)
    {
        if (cat.name == selectedItem) {
            QString fullPath = "/Libraries/" + getFullPath(cat.item);
            client->currentPath = fullPath;
            client->sendRequest();
            currentCatalog = cat;
        }
    }
   /* QStandardItem* item = model->itemFromIndex(index);
    QString fullPath = "/Libraries" + getFullPath(item);*/

   /* client->currentPath = fullPath;
    client->sendRequest();
    currentItem = item;*/
}

//Обновление QTreeView после получения данных с сервера
void LibrariesWidget::updateTree(const nlohmann::json& jsonData)
{
    if (client->currentPath == "/Libraries")
    {
        addJsonToModel(jsonData, root);
        return;
    }
    addLibraryToModel(jsonData, currentLibrary.item);
}

QString LibrariesWidget::getFullPath(QStandardItem* item)
{
    QStringList pathComponents;
    QStandardItem* current = item;

    while (current != nullptr)
    {
        QString name = current->text();
        if (name != " ")
            pathComponents.prepend(name);
        current = current->parent();
    }

    return "/" + pathComponents.join("/");
}

void LibrariesWidget::addJsonToModel(const nlohmann::json& jsonObj, QStandardItem* parentItem)
{
    parentItem->removeRows(0, parentItem->rowCount());
        /*QString root_name = QString::fromStdString(jsonObj["name"].get<std::string>());
        root->setText(root_name);*/

    if (jsonObj.contains("libraries") && jsonObj["libraries"].is_array())
    {
        for (const auto& lib : jsonObj["libraries"]) {
            Library library;
            library.name = QString::fromStdString(lib["name"].get<std::string>());
            library.dir = QString::fromStdString(lib["dir"].get<std::string>());
            library.ver = lib["ver"].get<double>();

            library.item = new QStandardItem(library.name);
            library.item->setIcon(QIcon::fromTheme("folder"));

            QStandardItem* dummyChild = new QStandardItem(" ");
            library.item->appendRow(dummyChild);

            parentItem->appendRow(library.item);
            libraries->append(library);
        }
    }
    firstRequest = false;
}

void LibrariesWidget::addLibraryToModel(const nlohmann::json& jsonObj, QStandardItem* parentItem)
{
    parentItem->removeRows(0, parentItem->rowCount());
    currentLibrary.components_location = QString::fromStdString(jsonObj["components_location"].get<std::string>());
    currentLibrary.layouts_location = QString::fromStdString(jsonObj["layouts_location"].get<std::string>());
    currentLibrary.sparam_location = QString::fromStdString(jsonObj["sparam_location"].get<std::string>());
    currentLibrary.symbols_location = QString::fromStdString(jsonObj["symbols_location"].get<std::string>());
    currentLibrary.thumbnails_location = QString::fromStdString(jsonObj["thumbnails_location"].get<std::string>());
    currentLibrary.ugos_location = QString::fromStdString(jsonObj["ugos_location"].get<std::string>());
    currentLibrary.veriloga_location = QString::fromStdString(jsonObj["veriloga_location"].get<std::string>());
    for (const auto& cat : jsonObj["catalogs"]) {
        Catalog catalog;
        catalog.name = QString::fromStdString(cat["name"].get<std::string>());
        catalog.thumb = QString::fromStdString(cat["thumb"].get<std::string>());

        catalog.item = new QStandardItem(catalog.name);

        /*if (catalog.components.isEmpty())
        {*/
            QStandardItem* dummyChild = new QStandardItem(" ");
            catalog.item->appendRow(dummyChild);
        /*}*/

        parentItem->appendRow(catalog.item);
        catalogs->append(catalog);
    }
}
//void LibrariesWidget::addJsonToModel(const nlohmann::json& jsonObj, QStandardItem* parentItem)
//{
//    parentItem->removeRows(0, parentItem->rowCount());
//
//    if (jsonObj.contains("files") && jsonObj["files"].is_array())
//    {
//        for (const auto& file : jsonObj["files"]) {
//            QString nameStr = QString::fromStdString(file["name"].get<std::string>());
//            QString iconStr = QString::fromStdString(file["icon"].get<std::string>());
//            QString typeStr = QString::fromStdString(file["type"].get<std::string>());
//
//            QIcon icon = convertSvgToIcon(iconStr);
//            QStandardItem* nameItem = new QStandardItem(nameStr);
//            nameItem->setIcon(icon);
//
//            if (typeStr == "directory")
//            {
//                QStandardItem* dummyChild = new QStandardItem(" ");
//                nameItem->appendRow(dummyChild);
//            }
//            parentItem->appendRow(nameItem);
//        }
//    }
//}


QIcon LibrariesWidget::convertSvgToIcon(QString svgString)
{
    QByteArray svgData = svgString.toUtf8();
    QSvgRenderer renderer(svgData);

    // Размер иконки
    int iconSize = 64;

    QPixmap pixmap(iconSize, iconSize);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    renderer.render(&painter);

    return QIcon(pixmap);
}

void LibrariesWidget::handleError(const QString& errorString)
{
    QMessageBox::warning(this, QStringLiteral(u"Ошибка при получении данных: "), errorString);
}

void LibrariesWidget::refreshButtonClicked()
{
    client->sendRequest();
}