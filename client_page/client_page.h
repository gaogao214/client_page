#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_client_page.h"
#include "asio.hpp"
#include "down_json.h"

class client_page : public QMainWindow
{
    Q_OBJECT

public:
    client_page(QWidget *parent = nullptr);
    ~client_page();
    Ui::client_pageClass ui;

  /*  auto get_ui()
    {
        return ui;
    }*/

private slots:
    void request_connect();
    void get_list_from_server();

private:

    std::shared_ptr<down_json> p_;
    asio::io_context io_context;

    std::shared_ptr<std::thread> main_thread_ptr_;
};
