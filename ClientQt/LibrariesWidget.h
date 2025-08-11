#pragma once
#include "Client.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QListView>
#include <QStringListModel>

class QTextEdit;

class LibrariesWidget : public QWidget
{
    Q_OBJECT

public:
    LibrariesWidget(QWidget* parent = nullptr);
    ~LibrariesWidget();

private:
    QListView* listView;
    QStringListModel* model;
    Client* client;

public slots:
    void updateList(const QStringList& items);
    void handleError(const QString& errorString);
};