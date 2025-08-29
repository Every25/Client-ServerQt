#include "LibrariesWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QMessageBox>
#include <QSplitter>

QSize iconSize(64, 64);
QIcon defaultFolder = QIcon("./icons/folder.svg");

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
    treeView->setIconSize(iconSize);

    //создание и добавление кнопок
    refreshButton = new QPushButton(QIcon("icons/refresh.svg"), "", this);
    buttonLayout->addWidget(refreshButton);
    
    componentsTable = new ComponentsTable(this);  
    componentsTable->setIconSize(iconSize);

    QSplitter* splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(treeView);
    splitter->addWidget(componentsTable); 

    splitter->setSizes({400, 400 });

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(splitter);

    setLayout(mainLayout);

    //привязка сигналов
    connect(treeView, &QTreeView::clicked, this, &LibrariesWidget::RequestWithSelectedItem);
    connect(treeView, &QTreeView::expanded, this, &LibrariesWidget::RequestWithSelectedItem);

    //привязка сигналов для кнопок
    connect(refreshButton, &QPushButton::clicked, this, &LibrariesWidget::refreshButtonClicked);

    //первое отправление запроса
    updateTree(readJson());
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
            QString fullPath = "./Libraries/" + lib.dir;
            currentPath = fullPath;
            currentLibrary = lib;
            updateTree(readJson());
            componentsTable->setRowCount(0);
            return;
        }
    }
    foreach(const auto& catalog, *catalogs)
    {
        if (catalog.name == selectedItem) {
            currentCatalog = catalog;
            //Обновление данных компонентов таблицы
            if (!currentCatalog.components.isEmpty())
            {
                componentsTable->updateComponents(currentCatalog.components);
                return;
            }
            componentsTable->setRowCount(0);
            return;
        }
    }
}

//Обновление QTreeView после получения данных с сервера
void LibrariesWidget::updateTree(const nlohmann::json& jsonData)
{
    if (currentPath == "./Libraries")
    {
        addRootJsonToModel(jsonData, root);
        return;
    }
    addLibraryToModel(jsonData, currentLibrary.item);
}

void LibrariesWidget::addRootJsonToModel(const nlohmann::json& jsonObj, QStandardItem* parentItem)
{
    if (!firstRequest)
    {
        parentItem->removeRows(0, parentItem->rowCount());
    }

    if (jsonObj.contains("libraries") && jsonObj["libraries"].is_array())
    {
        for (const auto& lib : jsonObj["libraries"]) {
            Library library;
            library.name = QString::fromStdString(lib["name"].get<std::string>());
            library.dir = QString::fromStdString(lib["dir"].get<std::string>());
            library.ver = lib["ver"].get<double>();

            library.item = new QStandardItem(library.name);
            library.item->setIcon(defaultFolder);

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
    if (jsonObj.contains("catalogs") && jsonObj["catalogs"].is_array()) {
        for (const auto& catalogJson : jsonObj["catalogs"]) {
            Catalog catalog;
            CatalogFromJson(catalogJson, catalog, parentItem);
        }
    }
}

void LibrariesWidget::CatalogFromJson(const nlohmann::json& jsonObj, Catalog& catalog, QStandardItem* parentItem)
{
    catalog.name = QString::fromStdString(jsonObj["name"].get<std::string>());
    catalog.thumb = QString::fromStdString(jsonObj["thumb"].get<std::string>());

    catalog.item = new QStandardItem(catalog.name);
    QIcon icon;
    if (catalog.thumb != "")
    {
        iconPath = "./Libraries/" + currentLibrary.dir + "/" + currentLibrary.thumbnails_location + "/" + catalog.thumb + ".svg";
        if (QFile::exists(iconPath)) 
        {
            icon = QIcon(iconPath);
        }
        else 
        {
            icon = defaultFolder;
        }
    }
    else
    {
        icon = defaultFolder;
    }
    catalog.item->setIcon(icon);
    if (parentItem) {
        parentItem->appendRow(catalog.item);
    }

    //Обработка вложенных каталогов
    if (jsonObj.contains("catalogs") && jsonObj["catalogs"].is_array()) {
        for (const auto& subCatalogJson : jsonObj["catalogs"]) {
            Catalog subCat;
            CatalogFromJson(subCatalogJson, subCat, catalog.item);
            catalog.catalogs.push_back(subCat);
        }
    }

    //Обработка компонентов (если есть)
    if (jsonObj.contains("components") && jsonObj["components"].is_array()) {
        for (const auto& compJson : jsonObj["components"]) {
            Component comp;
            ComponentFromJson(compJson, comp);
            catalog.components.push_back(comp);
        }
    }
    catalogs->push_back(catalog);
}

void LibrariesWidget::ComponentFromJson(const nlohmann::json& jsonObj, Component& component)
{
    component.model = QString::fromStdString(jsonObj["model"].get<std::string>());
    component.desc = QString::fromStdString(jsonObj["desc"].get<std::string>());

    QIcon icon;
    if (jsonObj["thumb"] != "")
    {
        iconPath = "./Libraries/" + currentLibrary.dir + "/" + currentLibrary.thumbnails_location + "/" + QString::fromStdString(jsonObj["thumb"].get<std::string>()) + ".svg";
        if (QFile::exists(iconPath))
        {
            icon = QIcon(iconPath);
            component.thumb = icon;
        }
    }
}

nlohmann::json LibrariesWidget::readJson() {

    QFileInfo fileInfo(currentPath);

    if (!fileInfo.exists()) {
        QMessageBox::warning(this, "Ошибка", QStringLiteral(u"ресурсы не найдены"));
        return nlohmann::json();
    }

    if (fileInfo.isDir()) {
        QDir librariesDir(currentPath);
        QStringList jsonFiles = librariesDir.entryList(QStringList() << "*.json", QDir::Files);

        if (!jsonFiles.isEmpty()) {
            QString jsonFilePath = librariesDir.filePath(jsonFiles.first());
            QFile jsonFile(jsonFilePath);

            if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QByteArray jsonContent = jsonFile.readAll();
                nlohmann::json j = nlohmann::json::parse(jsonContent);
                jsonFile.close();
                return j;
            }
            else {
                QMessageBox::warning(this, QStringLiteral(u"Ошибка при открытии json файла: "), jsonFilePath);
            }
        }
        else {
            QMessageBox::warning(this, "Ошибка", QStringLiteral(u"json файл не найден"));
        }
    }
    return nlohmann::json();
}

void LibrariesWidget::refreshButtonClicked()
{
    updateTree(readJson());
    componentsTable->setRowCount(0);
}