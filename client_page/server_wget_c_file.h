#pragma once
#pragma once
#include "asio.hpp"
#include <iostream>
#include <fstream>
#include "common.h"
#include "GxJsonUtility.h"
#include "rapidjson/filereadstream.h"
#include <deque>

using namespace std;
using namespace asio::ip;

class wget_load_file :public std::enable_shared_from_this<wget_load_file>
{

public:
	wget_load_file(tcp::socket socket);

	void start();

	void recive_wget_c_file_name();   //接收断点续传文件的名字

	void recive_wget_c_file(const string& file);  //接收断点续传的内容

	void do_wget_c_file(const string& file_name);   //解析json文件

	void send_file();             //查看断点时  哪些文件需要重新上传

	filestruct::wget_c_file_info  wget_c_file_profile(string& name);//打开断点续传文件  解析json文件

	void  down_json_profile(string& name);//打开配置文件，并找到配置文件中的路径,查看路径下的文件或文件名   解析json文件
	
	std::string open_json_file(const std::string& json_name);//打开指定名称的json文本
	
private:

	inline size_t get_file_len(const std::string& filename)
	{
		size_t fsize = 0;
		ifstream infile(filename.c_str());
		if (!infile.is_open())
		{
			cout << "文件夹内没有此文件 >:" << filename << endl;
			return 0;
		}
		infile.seekg(0, ios_base::end);
		fsize = infile.tellg();//list.json文本的大小
		infile.close();

		return fsize;
	}

	inline string/*char**/ get_file_context(const std::string& filename, int remaining_total, int offset/*,int count,int read_bytes=65536*/)//文本的内容
	{

		std::string content{};
		std::string tmp{};
		std::fill(content.begin(), content.end(), 0);//清空

		fstream ifs(filename, std::ios::in | std::ios::binary);
		if (!ifs.is_open())
			return {};

		//ifs.seekg(offset+(count*65536), ios::cur);
		ifs.seekg(offset, ios::cur);

		while (std::getline(ifs, tmp))
		{
			content.append(tmp);
		}

		return content;
	}

	void write(const std::string& msg);       //多个文件时，把要上传的文件内容存入队列里   
	void do_write();                          //上传数据

	std::deque<std::string> write_msgs_;
	std::mutex write_mtx_;
	asio::ip::tcp::socket socket_;
	char refile_name[1024] = { 0 };//接收名字的长度
	size_t filelen = 0;         //名字的长度
	char refile_file_len[sizeof(size_t)] = { 0 };//接收名字的长度
	string file_path_name;//路径+文件名
	std::string send_wget_name_and_offset_len{};
	std::string count_file_buf;//读到的字符
	size_t file_size = 0;//文件的总大小
	string wget_file_string;//发送断点文件中的文件名下的内容
	string wget_c_file_text; //接收文件的内容
	int buf_len;   //接收文件内容的长度
	string wget_name;        //断点续传中的文件名
	int wget_offset;		//断点续传中的偏移量
	string name_and_offset_remaining;       /* 名字  偏移量  余下内容的长度 以及余下的内容拼在一起 */
	size_t remaining_total;                 //计算余下的长度
	filestruct::wget_c_file_info wcfi;			//结构体实例化
	std::string down_json_name="down.json";


};

class wget_c_file_accept
{
public:
	wget_c_file_accept(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint)
		:acceptor_(io_context, endpoint)
	{
		do_accept();
	}

	void do_accept()
	{
		acceptor_.async_accept(
			[this](std::error_code ec, asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					cout << "服务端端口 12313 和客户端端口 12313 连接成功\n";
					auto fs = std::make_shared<wget_load_file>(std::move(socket));
					fs->start();
					sessions.push_back(fs);
				}
				else
				{
					cout << "服务端端口 12313 和客户端端口 12313 连接失败\n";
				}
				do_accept();
			});
	}

private:
	std::vector<std::shared_ptr<wget_load_file>> sessions;
	asio::ip::tcp::acceptor acceptor_;

};
