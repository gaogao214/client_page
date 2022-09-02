#include "client_to_the_session.h"
#include <fstream>
#include "file_struct.h"
#include "request.hpp"
#include "response.hpp"

static constexpr std::size_t send_count_size=8192;

int client_to_the_session::read_handle(uint32_t id)
{

	switch (id)
	{
	case request_number::id_name_request:

		id_name_request req;
		req.parse_bytes(buffer_);
		auto _id = req.body_.id_;
		auto file_name = req.body_.name_;
		OutputDebugStringA(file_name);
		OutputDebugString(L"接收成功\n");

		do_send_file(_id, file_name);
		Sleep(2);


		break;
	}


	return 0;
}

void client_to_the_session::do_send_file(uint32_t id,const std::string& filename)//发送数据
{
	std::size_t file_size = 0;
	std::string file_path_name = downfile_path.path + "\\" + filename;

	Sleep(2);

	std::ifstream file(file_path_name.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		return;

	file.seekg(0, std::ios_base::end);
	file_size = file.tellg();
	file.seekg(0, std::ios_base::beg);

	std::size_t nleft{};

	if (file_size > send_count_size)
	{
		std::size_t nchunkcount = file_size / send_count_size;

		if (file_size % nchunkcount != 0)
		{
			nchunkcount++;
		}
		for (std::size_t i = 0; i < nchunkcount; i++)
		{
			if (i + 1 == nchunkcount)
			{
				nleft = file_size - send_count_size * (nchunkcount - 1);

			}
			else
			{
				nleft = send_count_size;

			}
			std::unique_ptr<char[]> count_file_buf(new char[nleft]);

			file.seekg(i * send_count_size, std::ios::beg);
			file.read(count_file_buf.get(), nleft);

			id_text_response it_resp;

			it_resp.header_.length_ = nleft;
			std::memcpy(it_resp.header_.name_, filename.data(), filename.size());
			it_resp.header_.totoal_ = nchunkcount;
			it_resp.body_.id_ = id;
			it_resp.body_.set_text_(count_file_buf.get());

			this->async_write(std::move(it_resp), [this, filename](std::error_code ec, std::size_t sz)
				{
					if (!ec)
					{
						OutputDebugString(L"\n发送成功\n");
					}
				});
		}

	}
	else if (file_size < send_count_size)
	{
		nleft = file_size;
		std::unique_ptr<char[]> count_file_buf(new char[nleft]);

		file.read(count_file_buf.get(), nleft);

		id_text_response it_resp;

		std::memcpy(it_resp.header_.name_, filename.data(), filename.size());
		it_resp.header_.length_ = nleft;
		it_resp.body_.id_ = id;
		it_resp.body_.set_text_(count_file_buf.get());

		this->async_write(std::move(it_resp), [this, filename](std::error_code ec, std::size_t sz)
			{
				if (!ec)
				{
					OutputDebugString(L"\n");
					OutputDebugStringA(filename.data());
					OutputDebugString(L"\n");

				}
			});
	}

	file.close();

}
