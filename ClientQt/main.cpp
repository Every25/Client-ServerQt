#include "LibrariesWidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    LibrariesWidget window;
    window.setWindowTitle(QStringLiteral(u"Библиотеки элементов"));
    window.show();
    window.resize(600, 600);
    return app.exec();
}
