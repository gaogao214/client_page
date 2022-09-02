#pragma once
#pragma once
#include "asio.hpp"
#include <iostream>
#include <fstream>
#include <deque>
#include "file_struct.h"
#include "basic_session.hpp"
using namespace std;
using namespace asio::ip;

class server_wget_load_file_session 
	:public basic_session
{

public:
	server_wget_load_file_session(tcp::socket socket)
		:basic_session(std::move(socket))
	{

	}

private:
	void send_file();

protected:
	virtual int read_handle(uint32_t id)  override;


private:
	std::deque<std::string> write_msgs_;

	std::mutex write_mtx_;

	std::size_t nchunkcount_;


};

