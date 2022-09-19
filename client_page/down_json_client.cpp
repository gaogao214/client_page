#include <filesystem>
#include "request.hpp"
#include "down_json_client.h"
#include "down_block_client.h"
#include "response.hpp"
#include <string.h>

static constexpr char list_name[32] = "list.json";
static constexpr char id_name[32] = "id.json";
static constexpr char wget_c_file1[32] = "wget_c_file.json";

int down_json_client::read_handle(uint32_t id)
{
	
	switch (id)
	{
	case response_number::name_text_response_:
		
		name_text_response resp;

		resp.parse_bytes(buffer_);

		int list_ = strcmp(resp.header_.name_, list_name);
		int id_ = strcmp(resp.header_.name_, id_name);
	

		emit sign_file_name(resp.header_.name_);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(200ms);

		if (list_==0)
		{
			parse_server_list_json(resp.body_.text_);

			isfile_exist(resp.body_.text_, strlen(resp.body_.text_));

			emit sign_pro_bar(resp.header_.totoal_length_, strlen(resp.body_.text_));

			std::string str = resp.header_.name_ + std::string(u8"接收成功\n");
			QString qstr = QString::fromStdString(str);

			emit sign_text_log(qstr);
		}
		if (id_==0)
		{
			save_file(resp.header_.name_, resp.body_.text_);

			emit sign_pro_bar(resp.header_.totoal_length_, strlen(resp.body_.text_));

			std::string str = resp.header_.name_ + std::string(u8"接收成功\n");
			QString qstr = QString::fromStdString(str);

			emit sign_text_log(qstr);

			parse_block_json(resp.body_.text_);

			down_load();
		}
		break;	
	}

	return 0;
}

int down_json_client::read_error()
{
	return 0;
}

void down_json_client::isfile_exist(const char* file_buf, int buf_len)
{
	std::fstream list(list_name, std::ios::binary | std::ios::out | std::ios::app);
	if (!list.is_open())
	{
		save_file(list_name, file_buf);
	}
	else {
		parse_client_list_json(list_name);

		save_file(list_name, file_buf);
	}
}


void down_json_client::down_load()
{
	for (auto i : files_inserver.file_list)
	{
		id_index[i.blockid] += 1;
	}

	for (auto& iter : files_inserver.file_list)
	{
		
		auto it_client = std::find_if(files_inclient.file_list.begin(), files_inclient.file_list.end(), [&](auto file) {return file.path == iter.path; });

		if (it_client == files_inclient.file_list.end() || it_client->version < iter.version)
		{

			blks.blocks_[iter.blockid].id = iter.blockid;

			blks.blocks_[iter.blockid].files.push_back(iter.path);

			index[iter.blockid] += 1;      

			if (id_index[iter.blockid] == index[iter.blockid])
			{
				auto it = blks_.blocks.find(blks.blocks_[iter.blockid].id);

				if (it == blks_.blocks.end())
					continue;

				QVariant var;
				var.setValue(blks.blocks_[iter.blockid]);
		
				emit sign_down_block(var, iter.blockid, it->second.server.back().ip.data(), it->second.server.back().port.data());
				
			}
		}
	}
}

void down_json_client::send_id_port(std::size_t id, std::string port)
{
	
	id_port_request req;
	req.body_.id_ = id;
	req.body_.set_port(port);
	
	this->async_write(std::move(req), [this](std::error_code ec, std::size_t)
		{
			if (!ec)
			{
				std::string str(u8" ip port 发送成功 \n");
				QString qstr = QString::fromStdString(str);
				emit sign_text_log(qstr);
			}
		}); 

}
