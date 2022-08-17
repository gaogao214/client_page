#include "client_page.h"
#include <QtWidgets/QApplication>
#include <QStyleFactory>
#include <QVariant>
int main(int argc, char *argv[])
{
  
    QApplication a(argc, argv);
    client_page w;
    w.setWindowIcon(QIcon("images/client.ico"));
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    w.show();

    return a.exec();
}
