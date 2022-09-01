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
#include "file_struct.h"
#include "io_context_pool.h"
class down_block_client;
extern  filestruct::profile downfile_path;      //配置文件
extern  filestruct::files_info files_inclient;	//解析客户端本地的json文本


Q_DECLARE_METATYPE(filestruct::block)
class down_json_client 
	: public basic_client
{
	Q_OBJECT
public:
	down_json_client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints)
		: basic_client(io_context, endpoints)
		, pool(2)
		//, io_pool_block_(2)
	{
		parse_down_jsonfile(down_json_name);
	}

public:

	
	/*void set_funtion(std::function<void()>&& func)
	{
		func_.swap(func);
	}*/

protected:
	virtual int read_handle(uint32_t id) override;
	virtual int read_error() override;

private:
	void  parse_server_list_json(const char* text_json);//打开list_json   json文件  解析json文件
	
	void  parse_down_jsonfile(std::string name);//打开配置文件，并找到配置文件中的路径,查看路径下的文件或文件名   解析json文件

	void  parse_block_json(const char* text_json);//打开list_json   json文件  解析json文件

	void parse_client_list_json(std::string name);//打开list_json   json文件  解析json文件

	void isfile_exist(const char* file_buf, int buf_len);//判断list.json文件是否存在,存在就解析json文本与server的json进行比较，不存在就保存文件

	std::string open_json_file(const std::string& json_name);//打开指定名称的json文本

	void save_file(const char* name, const char* file_buf);//保存内容

	void down_load();//把任务放在线程池里向服务器请求下载

public:
	void send_id_port(/*const std::string id_port*/std::size_t id,std::string port);//发送成为服务器的id ip port 

signals:

	void sign_pro_bar(int maxvalue_, int value_);
	void sign_file_name(QString file_name);
	void sign_text_log(QString log_);
	void sign_down_block(QVariant var,QString loadip, QString loadport);


private:
	std::vector<std::shared_ptr<down_block_client>> down_block_list_;



public:
	std::string wget_c_file = "wget_c_file.json";
	std::string wget_c_file1 = "wget_c_file1.json";

private:
	std::string list_name;						//接收llist.json文件名
	std::string id_;					//接收id.json文件名

	filestruct::files_info files_inserver;		//解析服务器的json文本

	filestruct::blocks blks_;					//解析id.json文本
	filestruct::blocks_for_download blks;		//存一个块id的文件名


	char list_json[32] = "list.json";
	char id_json[32] = "id.json";
	char down_json_name[32] = "down.json";

	volatile int len = 0;
	std::unordered_map<int, int> id_index;		//一共的id  数量
	std::unordered_map<int, int> index;			//下载的次数

	std::string id_port_buf;						//发送的id和port的长度和内容
	ThreadPool pool;



	std::function<void()> func_;
};