#pragma once
#include "Client.h"

#include <QWidget>
#include <QListView>
#include <QStringListModel>
#include <QKeyEvent>

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

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

public slots:
    void updateList(const QStringList& items);
    void handleError(const QString& errorString);
    void RequestWithSelectedItem();
};