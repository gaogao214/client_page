#pragma once
#include "basic_client.h"
#include <memory>
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <string>
#include <algorithm>
#include <fstream>
#include "ThreadPool.h"

extern  filestruct::profile downfile_path;      //配置文件
extern  filestruct::files_info files_inclient;	//解析客户端本地的json文本

class down_json_client : public basic_client
{
public:
	down_json_client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints)
		: basic_client(io_context, endpoints)
		, pool(2)
	{

	}

public:

	void receive_list(std::size_t length);

	void receive_id(std::size_t length);

protected:
	virtual int read_handle(std::size_t bytes_transferred) override;

private:
	void  parse_server_list_json(std::string& text_json)//打开list_json   json文件  解析json文件
	{
		files_inserver.deserializeFromJSON(text_json.c_str());
	}

	void  parse_down_jsonfile(std::string& name)//打开配置文件，并找到配置文件中的路径,查看路径下的文件或文件名   解析json文件
	{
		std::string readbuffer = open_json_file(name);
		downfile_path.deserializeFromJSON(readbuffer.c_str());
	}

	void  parse_block_json(std::string& text_json)//打开list_json   json文件  解析json文件
	{
		blks_.deserializeFromJSON(text_json.c_str());
	}

	void parse_client_list_json(std::string& name)//打开list_json   json文件  解析json文件
	{
		std::string readbuffer = open_json_file(name);
		files_inclient.deserializeFromJSON(readbuffer.c_str());
	}

	void split_buffer();

	void isfile_exist(const std::string file_buf, int buf_len);//判断list.json文件是否存在,存在就解析json文本与server的json进行比较，不存在就保存文件

	std::string open_json_file(const std::string& json_name);//打开指定名称的json文本

	void save_file(const std::string& name, const std::string& file_buf);//保存内容

	void down_json_run(filestruct::block Files, std::string loadip, std::string loadport, const std::string& down_id);//连接下载文件的端口

	void down_load();//把任务放在线程池里向服务器请求下载

	void send_id_port(const std::string id_port);//发送成为服务器的id ip port 

public:
	std::string wget_c_file = "wget_c_file.json";
	std::string wget_c_file1 = "wget_c_file1.json";

private:
	std::string list_name;						//接收llist.json文件名
	std::string id_name_text;					//接收id.json文件名

	filestruct::files_info files_inserver;		//解析服务器的json文本
	filestruct::files_info files_inclient;//解析客户端本地的json文本
	filestruct::blocks blks_;					//解析id.json文本
	filestruct::blocks_for_download blks;		//存一个块id的文件名


	std::string list_json = "list.json";
	std::string id_json = "id.json";
	std::string down_json_name = "down.json";

	volatile int len = 0;
	std::unordered_map<int, int> id_index;		//一共的id  数量
	std::unordered_map<int, int> index;			//下载的次数

	filestruct::profile downfile_path;
	std::string id_port_buf;						//发送的id和port的长度和内容
	ThreadPool pool;
};