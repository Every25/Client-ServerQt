#include "ClientQt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ClientQt window;
    window.setWindowTitle("Client");
    window.show();
    return app.exec();
}
