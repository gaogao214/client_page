#pragma once
#include "basic_client.h"
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include "ThreadPool.h"
#include "file_struct.h"
#include "io_context_pool.h"

static const char down_json_name[32] = "down.json";

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

protected:
	virtual int read_handle(uint32_t id) override;
	virtual int read_error() override;

private:

	void isfile_exist(const char* file_buf, int buf_len);//判断list.json文件是否存在,存在就解析json文本与server的json进行比较，不存在就保存文件

	void down_load();//把任务放在线程池里向服务器请求下载

public:
	void send_id_port(/*const std::string id_port*/std::size_t id,std::string port);//发送成为服务器的id ip port 

signals:

	void sign_pro_bar(int maxvalue_, int value_);
	void sign_file_name(QString file_name);
	void sign_text_log(QString log_);
	void sign_down_block(QVariant var,QString loadip, QString loadport);


private:
	filestruct::blocks_for_download blks;		//存一个块id的文件名

	std::unordered_map<int, int> id_index;		//一共的id  数量
	std::unordered_map<int, int> index;			//下载的次数

	ThreadPool pool;

};