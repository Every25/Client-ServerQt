#pragma once
#include "Client.h"

#include <QWidget>
#include <QTreeView>
#include <QKeyEvent>
#include <QPushButton>
#include <QStack>
#include <QStandardItemModel>
#include <QStandardItem>
#include <nlohmann/json.hpp>
#include <QSvgRenderer>
#include <QPixmap>
#include <QPainter>

class QTextEdit;

class LibrariesWidget : public QWidget
{
    Q_OBJECT

public:
    LibrariesWidget(QWidget* parent = nullptr);
    ~LibrariesWidget();

private:
    QTreeView* treeView;
    QStandardItemModel* model;
    QStandardItem* root;
    Client* client;
    QPushButton* refreshButton;
    QStandardItem* currentItem;
    QStack<QString> pathHistory;

    void addJsonToModel(const nlohmann::json& jsonObj, QStandardItem* parentItem);
    QIcon convertSvgToIcon(QString svgString);
    QString getFullPath(QStandardItem* item);

public slots:
    void updateTree(const nlohmann::json& jsonData);
    void handleError(const QString& errorString);
    void RequestWithSelectedItem(const QModelIndex& index);
    void refreshButtonClicked();
};