#pragma once
#include "asio.hpp"
#include "basic_client.h"

class wget_c_file_client : public basic_client
{
	Q_OBJECT
public:
	wget_c_file_client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints)
		: basic_client(io_context, endpoints)
	{
		
	}

public:
	void do_send_wget_file_name_text(std::string wget_file_name);

	void do_recive_wget_file(uint32_t id);


protected:
	virtual int read_handle(uint32_t id) override;

	virtual int read_error() override;

signals:
	void sign_wget_c_file_text_log(QString text_);

	//void sign_wget_c_file_pro_bar(int maxvalue,int value);

	void sign_wget_c_file_name(QString name);

private:

	std::map<std::string, std::size_t> name_num_map_;

	std::size_t count = 0;

};

