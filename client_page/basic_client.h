#pragma once
#include <array>
#include "asio.hpp"

class basic_client
{
public:
	explicit basic_client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints)
		:io_context_(io_context)
		, socket_(io_context)
	{
		do_connect(endpoints);
	}

public:
	template<typename _Handle>
	void async_write(std::array<char, 1024> buffer, _Handle&& handle)
	{
		asio::async_write(socket_, asio::buffer(buffer), std::forward<_Handle>(handle));
	}

	void close()
	{
		socket_.close();
	}

protected:
	virtual int read_handle() = 0;

private:
	void do_connect(asio::ip::tcp::resolver::results_type endpoints)
	{
		asio::async_connect(socket_, endpoints,
			[this](std::error_code ec, asio::ip::tcp::endpoint)
			{
				if (ec)
					return;

				do_read();
			});
	}

	void do_read()
	{
		asio::async_read(socket_, asio::buffer(buffer),
			[this](std::error_code ec, std::size_t)
			{
				read_handle();

				do_read();
			});
	}


protected:
	std::array<char,1024> buffer;

private:
	asio::io_context& io_context_;
	asio::ip::tcp::socket socket_;
};

