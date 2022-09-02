#pragma once
#include <array>
#include "asio.hpp"
#include <QObject>
#include <sstream>
template<typename _Session>
class basic_server :public QObject
{
	//Q_OBJECT
public:
	explicit basic_server(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint/*,server_page* sep_*/)
		:io_context_(io_context)
		, acceptor_(io_context, endpoint)
		, socket_(io_context)
	{
		do_accept();
	}

public:
	void close()
	{
		socket_.close();
	}

	void run()
	{
		io_context_.run();
	}
private:
	void do_accept()
	{
		acceptor_.async_accept(
			[this](std::error_code ec, asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					auto s = std::make_shared<_Session>(std::move(socket));
					s->start();
					sessions_.push_back(s);
				}

				do_accept();
			});
	}


protected:
	std::array<char, 8192 + 1024> buffer_;


private:
	asio::ip::tcp::socket socket_;
	asio::ip::tcp::acceptor acceptor_;
	asio::io_context& io_context_;

	std::vector<std::shared_ptr<_Session>> sessions_;
};

