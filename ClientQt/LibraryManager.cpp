#include "LibraryManager.h"

LibraryManager::LibraryManager(QWidget* parent)
    : QTreeView(parent)
{
    setupTree();
}

LibraryManager::~LibraryManager()
{
    delete libraries;
    delete catalogs;
}

void LibraryManager::setupTree()
{
    libraries = new QList<Library>();
    catalogs = new QList<Catalog>();
    model = new QStandardItemModel(this);
    root = model->invisibleRootItem();
    setModel(model);
    setHeaderHidden(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    installEventFilter(this);
}

//Обновление QTreeView после получения данных с сервера
void LibraryManager::updateTree(const nlohmann::json& jsonData)
{
    if (currentPath == "./Libraries")
    {
        addRootJsonToModel(jsonData, root);
        return;
    }
    if (currentLibrary->components_location.isEmpty())
    {
        addLibraryToModel(jsonData, currentLibrary->item);
    }
}

void LibraryManager::addRootJsonToModel(const nlohmann::json& jsonObj, QStandardItem* parentItem)
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

void LibraryManager::addLibraryToModel(const nlohmann::json& jsonObj, QStandardItem* parentItem)
{
    parentItem->removeRows(0, parentItem->rowCount());
    currentLibrary->components_location = QString::fromStdString(jsonObj["components_location"].get<std::string>());
    currentLibrary->layouts_location = QString::fromStdString(jsonObj["layouts_location"].get<std::string>());
    currentLibrary->sparam_location = QString::fromStdString(jsonObj["sparam_location"].get<std::string>());
    currentLibrary->symbols_location = QString::fromStdString(jsonObj["symbols_location"].get<std::string>());
    currentLibrary->thumbnails_location = QString::fromStdString(jsonObj["thumbnails_location"].get<std::string>());
    currentLibrary->ugos_location = QString::fromStdString(jsonObj["ugos_location"].get<std::string>());
    currentLibrary->veriloga_location = QString::fromStdString(jsonObj["veriloga_location"].get<std::string>());
    if (jsonObj.contains("catalogs") && jsonObj["catalogs"].is_array()) {
        for (const auto& catalogJson : jsonObj["catalogs"]) {
            Catalog catalog;
            CatalogFromJson(catalogJson, catalog, parentItem);
        }
    }
}

void LibraryManager::CatalogFromJson(const nlohmann::json& jsonObj, Catalog& catalog, QStandardItem* parentItem)
{
    catalog.name = QString::fromStdString(jsonObj["name"].get<std::string>());
    catalog.thumb = QString::fromStdString(jsonObj["thumb"].get<std::string>());

    catalog.item = new QStandardItem(catalog.name);
    QIcon icon;
    if (catalog.thumb != "")
    {
        iconPath = "./Libraries/" + currentLibrary->dir + "/" + currentLibrary->thumbnails_location + "/" + catalog.thumb + ".svg";
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

void LibraryManager::ComponentFromJson(const nlohmann::json& jsonObj, Component& component)
{
    component.model = QString::fromStdString(jsonObj["model"].get<std::string>());
    component.desc = QString::fromStdString(jsonObj["desc"].get<std::string>());

    QIcon icon;
    if (jsonObj["thumb"] != "")
    {
        iconPath = "./Libraries/" + currentLibrary->dir + "/" + currentLibrary->thumbnails_location + "/" + QString::fromStdString(jsonObj["thumb"].get<std::string>()) + ".svg";
        if (QFile::exists(iconPath))
        {
            icon = QIcon(iconPath);
            component.thumb = icon;
        }
    }
}

nlohmann::json LibraryManager::readJson() {

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

void LibraryManager::request()
{
    updateTree(readJson());
}
