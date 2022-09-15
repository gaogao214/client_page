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
	{
		parse_down_jsonfile(down_json_name);
	}

protected:
	virtual int read_handle(uint32_t id) override;
	virtual int read_error() override;

private:

	void isfile_exist(const char* file_buf, int buf_len);

	void down_load();

public:
	void send_id_port(std::size_t id,std::string port);

signals:

	void sign_pro_bar(int maxvalue_,int value_);
	void sign_file_name(QString file_name);
	void sign_text_log(QString log_);
	void sign_down_block(QVariant var, int id, QString loadip, QString loadport);


private:
	filestruct::blocks_for_download blks;		

	std::unordered_map<int, int> id_index;		
	std::unordered_map<int, int> index;			

	ThreadPool pool;

};