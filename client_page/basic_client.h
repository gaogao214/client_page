#pragma once
#include <array>
#include "asio.hpp"
#include <QObject>
#include <sstream>

static constexpr std::size_t size_ = 8192 + 1024 + sizeof(uint32_t);


class basic_client:public QObject
{
	
public:
	explicit basic_client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints)
		:io_context_(io_context)
		,socket_(io_context)
		,endpoint_(endpoints)
	{
		do_connect();
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
	}

	template<typename _Request, typename _Handle>
	void async_write(_Request&& req, _Handle&& handle)
	{
		constexpr auto id = _Request::Number;

		std::array<char, size_> arr{};

		std::memcpy(arr.data(), &id, sizeof(uint32_t));

		req.to_bytes(arr.data() + sizeof(uint32_t));

		async_write(arr, std::forward<_Handle>(handle));
	}

	void close()
	{
		if (socket_.is_open())
		{
			asio::error_code ec;

			socket_.shutdown(asio::socket_base::shutdown_both, ec);

			socket_.close();
		}
	}

	auto& get_io_context()
	{
		return io_context_;
	}


protected:
	virtual int read_handle(uint32_t) = 0;

	virtual int read_error() = 0;


private:

	void do_connect()
	{
		connect_flag_ = false;

		if (!connect_flag_)
		{
			asio::async_connect(socket_, endpoint_,
				[&, this](std::error_code ec, asio::ip::tcp::endpoint)
				{
					if (ec)
					{		
						read_error();

						close();

						return;
					}
					connect_flag_ = true;		

					do_read_header();
				});
		}
	}

	void do_read_header()
	{		
		asio::async_read(socket_, asio::buffer(buffer_, sizeof(uint32_t)),
			[this](std::error_code ec, std::size_t sz)
			{
				if (ec)
				{
					close();

					do_connect();

					return ;
				}

				uint32_t proto_id{};

				std:memcpy(&proto_id, buffer_.data(), sizeof(uint32_t));
				//buffer_.fill(0);
				
				std::string pro = std::to_string(sz);

				OutputDebugStringA(std::string(buffer_.data(),4).data());
				OutputDebugStringA(std::to_string(buffer_[0]).data());
				OutputDebugStringA(std::to_string(buffer_[1]).data());
				OutputDebugStringA(std::to_string(buffer_[2]).data());
				OutputDebugStringA(std::to_string(buffer_[3]).data());
				OutputDebugStringA("\n");

				do_read_body(proto_id);
				
			});
	}

	void do_read_body(uint32_t id)
	{		
		socket_.async_read_some(asio::buffer(buffer_, size_ - sizeof(uint32_t)),
			[&, this](std::error_code ec, std::size_t bytes_transferred)
			{
				if (ec)
				{
					close();

					return;
				}
				
				
				OutputDebugStringA(buffer_.data());
				OutputDebugStringA("do_read_body_\n");

				read_handle(id);

				buffer_.fill(0);
			
				do_read_header();
			});
	}

protected:
	std::array<char, size_> buffer_;

public:
	bool connect_flag_=false;

private:
	asio::ip::tcp::resolver::results_type endpoint_;

	asio::io_context& io_context_;

	asio::ip::tcp::socket socket_;

	std::size_t file_size=0;

};

