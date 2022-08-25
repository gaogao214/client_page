#pragma once
#include <asio.hpp>
#include "basic_client.h"

#include "down_json_client.h"


class down_block_client : public basic_client 
{
public:
	down_block_client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints, const filestruct::block& block = {})
		: basic_client(io_context, endpoints)
		, blk(block)
	{
		
	}

public:

	void send_filename();

	void recive_file_text(size_t recive_len);

protected:

	virtual int read_handle(std::size_t bytes_transferred) override;
	virtual int read_error() override;

public:


	void does_the_folder_exist(const std::string& list_name);//判断文件夹是否存在，不存在则创建文件夹

	void client_to_server(std::string profile_port);    //开一个线程，客户端转换成服务端

	void server(const std::string& server_port);      //客户端转换成服务端

	filestruct::wget_c_file_info  parse_wget_c_file_json(const std::string& name)//打开断点续传文件  解析json文件
	{
		std::string readbuffer = open_json_file(name);

		wcfi.deserializeFromJSON(readbuffer.c_str());

		return wcfi;
	}


	std::string open_json_file(const std::string& json_name)//打开指定名称的json文本
	{
		std::string content{};
		std::string tmp{};
		std::fill(content.begin(), content.end(), 0);//清空

		std::fstream ifs(json_name, std::ios::in | std::ios::binary);
		if (!ifs.is_open())
			return {};

		while (std::getline(ifs, tmp))
		{
			content.append(tmp);
		}

		return content;

	}
	void save_location(const std::string& name, const std::string& no_path_add_name);    /*记录暂停下载时的  文件名以及偏移量  */

	void save_wget_c_file_json(filestruct::wget_c_file_info wcfi, const std::string& name);

	void Breakpoint_location();    /*记录暂停下载时的  文件名以及偏移量  */

	void gsh(std::string& strtxt);//按照格式写入id.json 文件


private:
	filestruct::block blk;//feige
	filestruct::wget_c_file wcf;

	filestruct::wget_c_file_info wcfi;  //声明一个结构体

	filestruct::wget_c_file_info wcfi_copy;  //声明一个结构体

	//down_json_client dj;       //声明一个down_json类

	int downloadingIndex = 0;//下载完的个数
	std::string file_name;
	std::string file_path;   //路径+文件名
	std::string name;




	//名字   总序号   内容
	std::string read_name;     //名字    name
	std::string file_path_;   //带有路径的名字   path + name
	std::vector<std::string> vec_text_;
	//std::map<std::string, std::vector<std::string>> num_text_;
	//std::map<std::string, std::map<std::string, std::vector<std::string>>> map_;
	std::map<std::string, std::size_t> map_;
	std::string texts_;
	std::size_t count = 0;
	//std::array<char, 5> arr{ 'a','a','a','a','a' };



	std::deque<std::string> write_msgs_;
	//std::deque<char *> write_msgs_;
	//std::deque<std::array<char, 8192 + 1024>> write_msgs_;

	std::mutex write_mtx_;

};

