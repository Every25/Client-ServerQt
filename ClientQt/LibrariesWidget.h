#pragma once
#include "Client.h"
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
    Client* client;
    QPushButton* refreshButton;
    QString currentPath = "/Libraries";
    QString iconPath;
    QIcon currentIcon;
    Library currentLibrary;
    Catalog currentCatalog;
    bool firstRequest = true;
    QList<Library>* libraries;
    QList<Catalog>* catalogs;

    void addRootJsonToModel(const nlohmann::json& jsonObj, QStandardItem* parentItem);
    void addLibraryToModel(const nlohmann::json& jsonObj, QStandardItem* parentItem);
    QIcon convertSvgToIcon(QString svgString);
    void ComponentFromJson(const nlohmann::json& j, Component& component);
    void CatalogFromJson(const nlohmann::json& jsonObj, Catalog& catalog, QStandardItem* parentItem);

public slots:
    void iconsFromJson(const nlohmann::json& jsonObj);
    void updateTree(const nlohmann::json& jsonData);
    void handleError(const QString& errorString);
    void RequestWithSelectedItem(const QModelIndex& index);
    void refreshButtonClicked();
};