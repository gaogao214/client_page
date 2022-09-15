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
		if (list_==0)
		{
			OutputDebugString(L"list.json 接收成功\n");
			parse_server_list_json(resp.body_.text_);
			isfile_exist(resp.body_.text_, strlen(resp.body_.text_));//判断list.json文件是否存在,存在就解析json文本与server的json进行比较，不存在就保存文件

		}
		if (id_==0)
		{
			OutputDebugString(L"id.json 接收成功\n");

			save_file(resp.header_.name_, resp.body_.text_);//保存内容
			parse_block_json(resp.body_.text_);
			down_load();//把任务放在线程池里向服务器请求下载

		}

		break;	
	}
	return 0;
}


int down_json_client::read_error()
{

	return 0;
}

void down_json_client::isfile_exist(const char* file_buf, int buf_len)//判断list.json文件是否存在,存在就解析json文本与server的json进行比较，不存在就保存文件
{

	std::fstream list(list_name, std::ios::binary | std::ios::out | std::ios::app);
	if (!list.is_open())
	{
		while (file_buf != nullptr)
		{
			list.write(file_buf, buf_len);
		}
		list.flush();
		list.close();
	}
	else {
		parse_client_list_json(list_name);
		save_file(list_name, file_buf);//保存内容
	}
}


void down_json_client::down_load()//把任务放在线程池里向服务器请求下载
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

				if (it == blks_.blocks.end())//没有找到
					continue;
				OutputDebugString(L"转文件io_context");

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
				
			}
		}); 

}
