#include "LibrariesWidget.h"
#include "Client.h"
#include <QtWidgets/QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{

    QApplication::setStyle(QStyleFactory::create("Windows"));
    QApplication app(argc, argv);
    LibrariesWidget window;
    window.setWindowTitle(QStringLiteral(u"Библиотеки элементов"));
    window.show();
    window.resize(600, 600);
    return app.exec();
}
