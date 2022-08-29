#pragma once
#include <QtWidgets/QMainWindow>
#include <QThread>
#include <QString>
#include "ui_client_page.h"
#include "asio.hpp"


#include "io_context_pool.h"
#include "file_struct.h"
#include "down_block_client.h"
#include "wget_c_file_client.h"


class client_page : public QMainWindow
{
	Q_OBJECT

public:
	client_page(QWidget* parent = nullptr);
	~client_page();




public:
	Ui::client_pageClass ui;
	size_t next_io_context_ = 0;
	/*  auto get_ui()
	  {
		  return ui;
	  }*/


public slots:

	void send_get_id_port_for_server(std::size_t get_server_id, QString get_server_port);
	void request_connect();
	void down_block_file_(QVariant file_names, QString loadip, QString loadport);
	void wget_c_file_();
	void show_progress_bar(int maxvalue_, int value_);
	void show_file_name(/*char file_name[512]*/QString file_name);
	void show_text_log(QString log_);
	void show_connect();

private:
	//std::shared_ptr<down_json_client> down_json_ptr_;
	down_json_client* down_json_ptr_;
	std::shared_ptr<wget_c_file_client> m_wget_c_file_;
	//std::shared_ptr<down_block_client> down_block_ptr_;
	std::vector<std::shared_ptr<down_block_client>> down_blocks_;

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

	void stop()
	{
		this->io_pool_.stop();

		pool_thread_ptr_->join();
	}


public:
	io_context_pool io_pool_;

private:
	std::shared_ptr<std::thread> pool_thread_ptr_;
};
