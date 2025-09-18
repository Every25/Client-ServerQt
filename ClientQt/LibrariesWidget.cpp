#include "LibrariesWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QMessageBox>
#include <QSplitter>

QSize iconSize(64, 64);

LibrariesWidget::LibrariesWidget(QWidget* parent)
    : QWidget(parent)
{
    libraryManager = new LibraryManager(this);
    libraryManager->setIconSize(iconSize);

    componentsTable = new ComponentsTable(this);
    componentsTable->setIconSize(iconSize);

    auto mainLayout = new QVBoxLayout(this);
    auto buttonLayout = new QHBoxLayout();

    //создание и добавление кнопок
    refreshButton = new QPushButton(QIcon("icons/refresh.svg"), "", this);
    buttonLayout->addWidget(refreshButton);

    QSplitter* splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(libraryManager);
    splitter->addWidget(componentsTable); 

    splitter->setSizes({400, 400 });

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(splitter);

    setLayout(mainLayout);

    //привязка сигналов
    connect(libraryManager, &QTreeView::clicked, this, &LibrariesWidget::RequestWithSelectedItem);
    connect(libraryManager, &QTreeView::expanded, this, &LibrariesWidget::RequestWithSelectedItem);

    //привязка сигналов для кнопок
    connect(refreshButton, &QPushButton::clicked, this, &LibrariesWidget::refreshButtonClicked);

    //первое отправление запроса
    libraryManager->request();
}

LibrariesWidget::~LibrariesWidget()
{

}

void LibrariesWidget::RequestWithSelectedItem(const QModelIndex& index)
{
    QString selectedItem = libraryManager->model->data(index, Qt::DisplayRole).toString();
    if (!index.isValid()) {
        return;
    }
    for(auto& lib : *libraryManager->libraries) {
        if (lib.name == selectedItem) {
            QString fullPath = "./Libraries/" + lib.dir;
            libraryManager->currentPath = fullPath;
            libraryManager->currentLibrary = &lib;
            libraryManager->request();
            componentsTable->setRowCount(0);
            return;
        }
    }
    for(auto& catalog : *libraryManager->catalogs)
    {
        if (catalog.name == selectedItem) {
            libraryManager->currentCatalog = &catalog;
            //Обновление данных компонентов таблицы
            if (!libraryManager->currentCatalog->components.isEmpty())
            {
                componentsTable->updateComponents(libraryManager->currentCatalog->components);
                return;
            }
            componentsTable->setRowCount(0);
            return;
        }
    }
}

void LibrariesWidget::refreshButtonClicked()
{
    libraryManager->request();
    componentsTable->setRowCount(0);
}