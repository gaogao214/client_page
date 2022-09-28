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



public slots:

	void init_checkedbox();

	void save_lose_sight_of_path();

	void save_forced_upload_path();

	void send_get_id_port_for_server(std::size_t get_server_id, QString get_server_port);

	void request_connect();
	
	void down_block_file_(QVariant file_names, int id, QString loadip, QString loadport);
	
	void wget_c_file_(QString wget_file_name);
	
	void show_progress_bar(int maxvalue_, int value_);
	
	void show_file_name(QString file_name);
	
	void show_text_log(QString log_);
	
	void add_lose_sight_of_listview(bool);

	void add_upload_listview(bool);

	void clear_log_();

	void delete_lose_signt_of_listwidget();

	void delete_upload_listwidget();

	void clear_lose_signt_of_listwidget();

	void clear_upload_listwidget();

	void start_upload_file();

	void selected_forced_updating_path();

	void selected_lose_signt_of_path();

	void forced_updating_path(std::vector<std::string> path);

private:
	
	

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

	std::vector<std::shared_ptr<down_block_client>> down_blocks_;

	wget_c_file_client* wget_c_file_client_ptr_;

	down_json_client* down_json_ptr_;

	filestruct::block bck;

	std::shared_ptr<std::thread> pool_thread_ptr_;

	filestruct::blocks_for_download choose_blks;

	int count_ = 0;

	std::unordered_map<int, int> id_index_;
	std::unordered_map<int, int> index_;
};
