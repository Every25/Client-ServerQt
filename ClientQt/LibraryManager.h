#pragma once

#include <QWidget>
#include <QTreeView>
#include <QHeaderView>
#include <QStringList>
#include <QMessageBox>
#include <QDir>

#include <nlohmann/json.hpp>

#include "Library.h"
#include "Catalog.h"
#include "Component.h"



class LibraryManager : public QTreeView
{
    Q_OBJECT

public:
    LibraryManager(QWidget* parent = nullptr);
    ~LibraryManager();

    QStandardItemModel* model;
    QList<Library>* libraries;
    QList<Catalog>* catalogs;
    QString currentPath = "./Libraries";
    Library* currentLibrary = nullptr;
    Catalog* currentCatalog = nullptr;

    void request();
private:
    QStandardItem* root;
    QIcon defaultFolder = QIcon("./icons/folder.svg");
    bool firstRequest = true;
    QString iconPath;

    void setupTree();
    void updateTree(const nlohmann::json& jsonData);
    void addRootJsonToModel(const nlohmann::json& jsonObj, QStandardItem* parentItem);
    void addLibraryToModel(const nlohmann::json& jsonObj, QStandardItem* parentItem);
    void CatalogFromJson(const nlohmann::json& jsonObj, Catalog& catalog, QStandardItem* parentItem);
    void ComponentFromJson(const nlohmann::json& jsonObj, Component& component);
    nlohmann::json readJson();
};