#pragma once

#include <Library.h>
#include <Catalog.h>
#include <Component.h>

#include <QWidget>
#include <QTreeView>
#include <QKeyEvent>
#include <QPushButton>
#include <QStandardItemModel>
#include <QStandardItem>
#include <nlohmann/json.hpp>
#include <QSvgRenderer>
#include <QPixmap>
#include <QPainter>
#include <QList>
#include "ComponentsTable.h"
#include <QDir>

class QTextEdit;

class LibrariesWidget : public QWidget
{
    Q_OBJECT

public:
    LibrariesWidget(QWidget* parent = nullptr);
    ~LibrariesWidget();

private:
    QTreeView* treeView;
    ComponentsTable* componentsTable;
    QStandardItemModel* model;
    QStandardItem* root;
    QPushButton* refreshButton;
    bool firstRequest = true;
    QList<Library>* libraries;
    QList<Catalog>* catalogs;
    QString currentPath = "./Libraries";
    QString iconPath;

    void addRootJsonToModel(const nlohmann::json& jsonObj, QStandardItem* parentItem);
    void addLibraryToModel(const nlohmann::json& jsonObj, QStandardItem* parentItem);
    void ComponentFromJson(const nlohmann::json& j, Component& component);
    void CatalogFromJson(const nlohmann::json& jsonObj, Catalog& catalog, QStandardItem* parentItem);
    nlohmann::json readJson();

public slots:
    void updateTree(const nlohmann::json& jsonData);
    void RequestWithSelectedItem(const QModelIndex& index);
    void refreshButtonClicked();
};