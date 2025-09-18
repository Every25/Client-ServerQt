#pragma once

#include "ComponentsTable.h"
#include "LibraryManager.h"

#include <QWidget>
#include <QKeyEvent>
#include <QPushButton>

class QTextEdit;

class LibrariesWidget : public QWidget
{
    Q_OBJECT

public:
    LibrariesWidget(QWidget* parent = nullptr);
    ~LibrariesWidget();

private:
    LibraryManager* libraryManager;
    ComponentsTable* componentsTable;
    QPushButton* refreshButton;

public slots:
    void RequestWithSelectedItem(const QModelIndex& index);
    void refreshButtonClicked();
};