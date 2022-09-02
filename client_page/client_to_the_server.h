#pragma once
#include "basic_server.h"
#include "client_to_the_session.h"

class client_to_the_server : public basic_server<client_to_the_session>
{
public:
	client_to_the_server(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint)
		:basic_server<client_to_the_session>(io_context,endpoint)
	{
		
	}

};
