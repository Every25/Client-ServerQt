#include "LibrariesWidget.h"
#include "Client.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    LibrariesWidget window;
    window.setWindowTitle(QStringLiteral(u"Библиотеки"));
    window.show();
    return app.exec();
}
