#include "client_page.h"
#include <QtWidgets/QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    client_page w;
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    w.show();

    return a.exec();
}
