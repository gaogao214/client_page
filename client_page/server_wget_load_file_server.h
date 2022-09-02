#pragma once
#include "asio.hpp"
#include "basic_server.h"
#include "server_wget_load_file_session.h"
class server_wget_load_file_server : public basic_server<server_wget_load_file_session>
{

public:
	server_wget_load_file_server(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint)
		:basic_server < server_wget_load_file_session > (io_context, endpoint)
		{
		
		}

};


