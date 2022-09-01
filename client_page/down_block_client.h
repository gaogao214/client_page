#pragma once
#include <asio.hpp>
#include <QObject>
#include "basic_client.h"
#include "down_json_client.h"


class down_block_client : public basic_client 
{
	Q_OBJECT
public:
	down_block_client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints, const filestruct::block& block = {})
		: basic_client(io_context, endpoints)
		, blk(block)
	{
		client_ = this;
	
	}

public:

	void send_filename();

	void recive_file_text(uint32_t id);

protected:

	virtual int read_handle(uint32_t id) override;
	virtual int read_error() override;

public:


	void does_the_folder_exist(const std::string& list_name);

	static void client_to_server(std::string profile_port);    

	static void server(const std::string& server_port);      

	static void save_location(const std::string& name, const std::string& no_path_add_name,std::size_t id_num);   

	void save_location_connect_error(const std::string& name, const std::string& no_path_add_name);

	static void save_wget_c_file_json(filestruct::wget_c_file_info wcfi, std::string name);


signals:
	
	void signal_get_id_port_externl(std::size_t get_server_id, QString get_server_port);
	void signal_pro_bar(int maxvalue, int value);
	void signal_file_name_(QString file_name_);


private:
	filestruct::block blk;

	std::string read_name;     
	std::string file_path_;   
	
	std::map<std::string, std::size_t> map_;
	std::size_t count = 0;

public:

	
	static std::mutex write_mtx_;
	static std::unordered_map<std::size_t, std::vector<std::string>> total_id_files_num;

	static filestruct::wget_c_file_info wcfi_copy;  //声明一个结构体
	static filestruct::wget_c_file wcf;
	static filestruct::block blk_copy;

	static down_block_client* client_;

};

