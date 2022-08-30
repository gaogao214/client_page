#pragma once
#include <array>
#include "asio.hpp"
#include <QObject>
#include <sstream>

#define SEND_COUNT_SIZE 1024*1024


class basic_client:public QObject
{
	//Q_OBJECT
public:
	explicit basic_client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints)
		:io_context_(io_context)
		, socket_(io_context)
	{
		do_connect(endpoints);
	}

public:
	template<std::size_t I, typename _Handle>
	void async_write(std::array<char, I> buffer, _Handle&& handle)
	{
		asio::async_write(socket_, asio::buffer(buffer), std::forward<_Handle>(handle));
	}

	template<typename _Handle>
	void async_write(const std::string& buffer, _Handle&& handle)
	{
		asio::async_write(socket_, asio::buffer(buffer), std::forward<_Handle>(handle));
		//socket_.async_write_some(asio::buffer(buffer), std::forward<_Handle>(handle));
	}

	template<typename _Request, typename _Handle>
	void async_write(_Request&& req, _Handle&& handle)
	{
		std::array<char, 1024> arr{};
		req.to_bytes(arr);

		async_write(arr, std::forward<_Handle>(handle));
	}

	void close()
	{
		socket_.close();
	}

	auto& get_io_context()
	{
		return io_context_;
	}

protected:
	virtual int read_handle(std::size_t) = 0;
	virtual int read_error() = 0;

//signals:
//	void signal_connect();

private:
	void do_connect(asio::ip::tcp::resolver::results_type endpoints)
	{
		asio::async_connect(socket_, endpoints,
			[this](std::error_code ec, asio::ip::tcp::endpoint)
			{
				if (ec)
				{
					read_error();
				}

				//emit signal_connect();
				do_read_header();
			});
	}

	void do_read_header()
	{		

		asio::async_read(socket_, asio::buffer(buffer_, sizeof(size_t)),
			[this](std::error_code ec, std::size_t)
			{
				if (ec)
				{
					read_error();
					return ;
				}
				
				std::size_t receive_length{};

				std::memcpy(&receive_length, &buffer_, sizeof(std::size_t));
				

				do_read_body(receive_length);
				
			});

	}

	void do_read_body(std::size_t length)
	{		

		asio::async_read(socket_, asio::buffer(buffer_, length), 
			[this](std::error_code ec, std::size_t bytes_transferred)
			{
				if (ec)
					return;

				read_handle(bytes_transferred);
				buffer_.fill(0);

				//std::memset(buffer_.data(), 0, 4096);//Çå¿ÕÄÚ´æ
				do_read_header();
			});
	}


protected:
	//std::array<char, 4096> buffer_;
	std::array<char, 8192+1024> buffer_;

private:
	asio::io_context& io_context_;
	asio::ip::tcp::socket socket_;
	std::size_t file_size=0;

};

