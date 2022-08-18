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

	template<typename _Handle>
	void async_write(const std::string& buffer, _Handle&& handle)
	{
		asio::async_write(socket_, asio::buffer(buffer), std::forward<_Handle>(handle));
	}

	void close()
	{
		socket_.close();
	}

protected:
	virtual int read_handle(std::size_t) = 0;

private:
	void do_connect(asio::ip::tcp::resolver::results_type endpoints)
	{
		asio::async_connect(socket_, endpoints,
			[this](std::error_code ec, asio::ip::tcp::endpoint)
			{
				if (ec)
					return;

				do_read_header();
			});
	}

	void do_read_header()
	{
		asio::async_read(socket_, asio::buffer(buffer_,sizeof(size_t)),
			[this](std::error_code ec, std::size_t)
			{
				auto receive_length = atoi(buffer_.data());

				do_read_body(receive_length);
			});
	}

	void do_read_body(std::size_t length)
	{
		asio::async_read(socket_, asio::buffer(buffer_, length), 
			[this](std::error_code ec, std::size_t bytes_transferred)
			{
				read_handle(bytes_transferred);

				do_read_header();
			});
	}


protected:
	std::array<char,1024> buffer_;

private:
	asio::io_context& io_context_;
	asio::ip::tcp::socket socket_;
};

