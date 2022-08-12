#pragma once

#include <QtWidgets/QMainWindow>
#include <QThread>
#include <QString>
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

    
public slots:

    void request_connect();
    void wget_c_file_();
    void show_progress_bar(int maxvalue_ ,int value_ );
    void show_file_name(/*char file_name[512]*/QString file_name);
    void show_text_log(QString log_);

private:

    std::shared_ptr<down_json> p_;
    asio::io_context io_context;
    asio::ip::tcp::resolver::results_type endpoints;

   // std::shared_ptr<std::thread> main_thread_ptr_;
    double dpro;
    std::shared_ptr<std::thread> connect_ptr_;
    QThread* thread_;
};
