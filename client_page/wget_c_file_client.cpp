#include "wget_c_file_client.h"
#include <iostream>
#include "request.hpp"
#include "response.hpp"
#include "file_struct.h"
static constexpr char wget_c_name2[32] = "wget_c_file2.json";

void wget_c_file_client::do_send_wget_file_name_text(std::string wget_file_name)
{

	std::size_t fsize = get_file_len(wget_file_name);
	std::string list_buf = get_file_context(wget_file_name);

	name_text_request req;
	req.header_.length_ = fsize;
	req.body_.set_name(wget_file_name,list_buf);


	this->async_write(std::move(req), [this](std::error_code ec, std::size_t sz)
		{
			if (!ec) 
			{
				OutputDebugString(L"s 断点续传文件发送成功");

			}
			OutputDebugStringA(ec.message().data());

		});
}


void wget_c_file_client::do_recive_wget_file(uint32_t id )
{

	switch (id)
	{
	case response_number::offset_text_response_:
		offset_text_response resp;
		resp.parse_bytes(buffer_);
		auto len = resp.header_.length_;
		auto name = resp.header_.name_;
		auto total_num = resp.header_.totoal_;
		auto offset_ = resp.body_.offset_;
		auto text = resp.body_.text_;

		name_num_map_.emplace(name, total_num);

		std::string file_name = downfile_path.path + "\\" + name;


		std::ofstream file(file_name.data(), std::ios::out | std::ios::binary | std::ios::app);

		for (auto iter = name_num_map_.begin(); iter != name_num_map_.end(); iter++)
		{
			if (iter->first == name)
			{
				
				file.seekp(offset_, std::ios::beg);

				file.write(text, len);
				++count;
				OutputDebugStringA(name);
				OutputDebugStringA("文件接收成功\n");


				if (iter->second == count)
				{
					file.close();

				}
			}
		}
		break;
	}
}

int wget_c_file_client::read_handle(uint32_t id)
{

	do_recive_wget_file(id);
	return 0;

}

int wget_c_file_client::read_error()
{

	return 0;
}
