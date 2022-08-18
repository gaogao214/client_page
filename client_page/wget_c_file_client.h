#pragma once
#include "asio.hpp"
#include "basic_client.h"
#include "down_json_client.h"
#include <fstream>
class wget_c_file_client : public basic_client
{
public:
	wget_c_file_client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints)
		: basic_client(io_context, endpoints)
	{

	}

public:
	void do_send_wget_file_name();//发送名字

	void do_send_wget_file_name_offset();//发送偏移长度

	void do_recive_wget_file();

protected:
	virtual int read_handle(std::size_t bytes_transferred) override;

private:
	inline size_t send_file_len(const std::string& filename)
	{
		std::ifstream infile(filename.c_str());
		infile.seekg(0, std::ios_base::end);
		size_t fsize = infile.tellg();//list.json文本的大小
		infile.close();

		return fsize;
	}

	inline std::string send_file_context(const std::string& filename)//文本的内容
	{

		std::ifstream File(filename.c_str());
		char file_buf = '0';//list.json文件
		std::string buf;//一个一个读之后存在这里，list.json文本

		while (File.get(file_buf))
		{
			buf.push_back(file_buf);
		}
		File.close();

		return buf;
	}

private:
	std::string wget_c_name = "wget_c_file.json";
	std::string send_name;						//发送断点续传的名字
	std::string wget_text;						//发送断点续传文件的内容
	std::string recive_wget_name;               //接收断点续传的文件名 偏移量 余下的长度 内容
};

