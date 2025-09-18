#include "ComponentsTable.h"

ComponentsTable::ComponentsTable(QWidget* parent)
    : QTableWidget(parent)
{
    setupTable();
}

void ComponentsTable::setupTable()
{
    setColumnCount(3);

    QStringList headers;
    headers << "Symbol" << "Model" << "Description";
    setHorizontalHeaderLabels(headers);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setAlternatingRowColors(true);
    setShowGrid(false);
    verticalHeader()->setVisible(false);

    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    setColumnWidth(0, 30);
    setColumnWidth(1, 150);
}

void ComponentsTable::updateComponents(const QList<Component>& components)
{
    // Очищаем таблицу
    setRowCount(0);

    // Добавляем компоненты в таблицу
    for (const auto& component : components)
    {
        int row = rowCount();
        insertRow(row);

        // Иконка (пока пустая ячейка)
        setItem(row, 0, new QTableWidgetItem());
        item(row, 0)->setIcon(component.thumb);

        // Модель
        setItem(row, 1, new QTableWidgetItem(component.model));

        // Описание
        setItem(row, 2, new QTableWidgetItem(component.desc));
    }

    // Автоподбор размера строк
    resizeRowsToContents();
}