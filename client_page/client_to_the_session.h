#pragma once
#include "asio.hpp"
#include <iostream>
#include "basic_session.hpp"
class client_to_the_session
	:public basic_session
{
public:
	client_to_the_session(asio::ip::tcp::socket socket)
		:basic_session(std::move(socket))
	{
	
	}

private:
	void do_send_file(uint32_t id,const std::string& fname);

protected:
	virtual int read_handle(uint32_t) override;

};
