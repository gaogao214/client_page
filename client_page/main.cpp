#include "client_page.h"
#include <QtWidgets/QApplication>
#include <QStyleFactory>
#include <QVariant>

#include <filesystem>

void clear()
{
    std::filesystem::remove("id.json");
    std::filesystem::remove("list.json");
    std::filesystem::remove("wget_c_file1.json");
    std::filesystem::remove("wget_c_file.json");
    std::filesystem::remove_all("down/");
}

int main(int argc, char *argv[])
{
    clear();

    QApplication a(argc, argv);
    client_page w;
    w.setWindowIcon(QIcon("images/client.ico"));
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    w.show();

    return a.exec();
}
