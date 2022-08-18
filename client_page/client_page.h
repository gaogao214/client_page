#pragma once
#include <QtWidgets/QMainWindow>
#include <QThread>
#include <QString>
#include "ui_client_page.h"
#include "asio.hpp"
#include "wget_c_file.h"
#include "down_block.h"


#include "io_context_pool.h"

class client_page : public QMainWindow
{
    Q_OBJECT

public:
    client_page(QWidget *parent = nullptr);
    ~client_page();

   


public:
    Ui::client_pageClass ui;
    size_t next_io_context_=0;
  /*  auto get_ui()
    {
        return ui;
    }*/

    
public slots:
   
    void request_connect();
    void down_block_file_(QVariant file_names,QString loadip, QString loadport);
    void wget_c_file_();
    void show_progress_bar(int maxvalue_ ,int value_ );
    void show_file_name(/*char file_name[512]*/QString file_name);
    void show_text_log(QString log_);

private:
    std::shared_ptr<down_json> down_json_ptr_;
    std::shared_ptr<wget_c_file> m_wget_c_file_;
    std::shared_ptr<down_block> down_block_;

    std::shared_ptr<std::thread> main_thread_ptr_;
    double dpro;

    filestruct::block bck;


public:
    void start_io_pool()
    {
        pool_thread_ptr_.reset(new std::thread([this]
            {
                this->io_pool_.run();
            }));
    }


private:
    io_context_pool io_pool_;

    std::shared_ptr<std::thread> pool_thread_ptr_;
};
