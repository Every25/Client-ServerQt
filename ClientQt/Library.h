#pragma once

#include <QStandardItem>
#include <Catalog.h>
#include <iostream>

struct Library
{
	QStandardItem* item;
	QString name;
	QString dir;
	double ver;
	QString components_location;
	QString layouts_location;
	QString sparam_location;
	QString symbols_location;
	QString thumbnails_location;
	QString ugos_location;
	QString veriloga_location;
	QList<Catalog> catalogs;
};