#pragma once

#include <QString>

struct Catalog
{
	QString name;
	QString thumb;
	QList<Catalog> catalogs;
};

