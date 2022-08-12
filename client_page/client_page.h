#pragma once

#include <QtWidgets/QMainWindow>
#include <QThread>
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

    void show_file_name(char file_name[512]);
    void init();
public slots:

    void request_connect();
    void wget_c_file_();
    void show_progress_bar(int maxvalue_ ,int value_ );


private:

    std::shared_ptr<down_json> p_;
    down_json* down_json_;
    asio::io_context io_context;
    asio::ip::tcp::resolver::results_type endpoints;
  //  client_page* cli_page_;
   // std::shared_ptr<std::thread> main_thread_ptr_;
    double dpro;
    std::shared_ptr<std::thread> connect_ptr_;
    QThread* thread_;
};
