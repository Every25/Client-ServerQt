#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QStringList>
#include "Component.h"
#include <nlohmann/json.hpp>


class ComponentsTable : public QTableWidget
{
    Q_OBJECT

public:
    ComponentsTable(QWidget* parent = nullptr);
    ~ComponentsTable() = default;

    void updateComponents(const QList<Component>& components);

private:
    void setupTable();
};