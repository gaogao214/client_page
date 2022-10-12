#include "wget_c_file_client.h"
#include <iostream>
#include "request.hpp"
#include "response.hpp"
#include "file_struct.h"


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
				emit sign_wget_c_file_text_log(u8"断点续传文件发送成功 \n");
			}
		});
}

void wget_c_file_client::do_recive_wget_file(uint32_t id )
{
	switch (id)
	{
	case uint32_t(response_number::offset_text_response_):

		offset_text_response resp;

		resp.parse_bytes(buffer_);
		
		emit sign_wget_c_file_name(resp.header_.name_);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(200ms);

		name_num_map_.emplace(resp.header_.name_, resp.header_.totoal_sequence_);

		std::string file_name = downfile_path.path + "\\" + resp.header_.name_;

		std::ofstream file(file_name.data(), std::ios::out | std::ios::binary | std::ios::app);

		for (auto iter = name_num_map_.begin(); iter != name_num_map_.end(); iter++)
		{
			if (iter->first == resp.header_.name_)
			{
				file.seekp(resp.body_.offset_, std::ios::beg);

				file.write(resp.body_.text_, resp.header_.length_);

				++count;
					
				//emit sign_wget_c_file_pro_bar(resp.header_.totoal_length_,resp.header_.length_ * count);


				if (iter->second == count)
				{
					file.close();

					std::string str= resp.header_.name_+std::string(u8"接收成功\n");
					QString qstr = QString::fromStdString(str);

					emit sign_wget_c_file_text_log(qstr);
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
