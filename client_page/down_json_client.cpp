#include "request.hpp"
#include "down_json_client.h"
#include "down_block_client.h"
#include "file_server.h"
#include "response.hpp"
#include <string.h>
filestruct::profile downfile_path;
filestruct::files_info files_inclient;//解析客户端本地的json文本
		


int down_json_client::read_handle(uint32_t id)
{
	//构造response
	switch (id)
	{
	case 1001:
		
		name_text_response resp;
		char list_name[32] = "list.json";
		char id_name[32] = "id.json";
		resp.parse_bytes(buffer_);

		int list_ = strcmp(resp.body_.name_, list_name);
		int id_ = strcmp(resp.body_.name_, id_name);
		if (list_==0)
		{
			OutputDebugString(L"list.json 接收成功\n");
			parse_server_list_json(resp.body_.text_);
			isfile_exist(resp.body_.text_, strlen(resp.body_.text_));//判断list.json文件是否存在,存在就解析json文本与server的json进行比较，不存在就保存文件
		}
		if (id_==0)
		{
			OutputDebugString(L"id.json 接收成功\n");

			save_file(resp.body_.name_, resp.body_.text_);//保存内容
			parse_block_json(resp.body_.text_);
			down_load();//把任务放在线程池里向服务器请求下载

		}

		break;
	/*case 1003:
		break;*/
	/*default:
		break;*/
	}

	//if (func_)
	//	func_();

	return 0;
}


int down_json_client::read_error()
{

	return 0;
}

void  down_json_client::parse_server_list_json(const char* text_json)//打开list_json   json文件  解析json文件
{
	files_inserver.deserializeFromJSON(text_json);
}

void  down_json_client::parse_down_jsonfile(std::string name)//打开配置文件，并找到配置文件中的路径,查看路径下的文件或文件名   解析json文件
{
	std::string readbuffer = open_json_file("down.json");
	downfile_path.deserializeFromJSON(readbuffer.c_str());
}

void  down_json_client::parse_block_json(const char* text_json)//打开list_json   json文件  解析json文件
{
	blks_.deserializeFromJSON(text_json);
}

void down_json_client::parse_client_list_json(std::string name)//打开list_json   json文件  解析json文件
{
	std::string readbuffer = open_json_file(name);
	files_inclient.deserializeFromJSON(readbuffer.c_str());
}


void down_json_client::isfile_exist(const char* file_buf, int buf_len)//判断list.json文件是否存在,存在就解析json文本与server的json进行比较，不存在就保存文件
{

	std::fstream list("list.json", std::ios::binary | std::ios::out | std::ios::app);
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
		parse_client_list_json(list_json);
		save_file(list_json, file_buf);//保存内容
	}
}

std::string down_json_client::open_json_file(const std::string& json_name)//打开指定名称的json文本
{
	std::string content{};
	std::string tmp{};
	std::fill(content.begin(), content.end(), 0);//清空

	std::fstream ifs(json_name, std::ios::in | std::ios::binary);
	if (!ifs.is_open())
		return {};

	while (std::getline(ifs, tmp))
	{
		content.append(tmp);
	}

	return content;

}
void down_json_client::save_file(const char* name, const char* file_buf)//保存内容
{

	std::ofstream save_file_(name, std::ios::out | std::ios::binary);

	save_file_.write(file_buf, strlen(file_buf) - len);

	save_file_.close();

}


void down_json_client::down_load()//把任务放在线程池里向服务器请求下载
{
	for (auto i : files_inserver.file_list)//每个id(id:1 id:2)的总数量
	{
		id_index[i.blockid] += 1;
	}

		
	//auto iter = *files_inserver.file_list.begin();
	for (auto& iter : files_inserver.file_list)
	{
		//在本地list.json文本里找到和服务端相同的名字
		auto it_client = std::find_if(files_inclient.file_list.begin(), files_inclient.file_list.end(), [&](auto file) {return file.path == iter.path; });
		if (it_client == files_inclient.file_list.end() || it_client->version < iter.version)//如果没有找到名字，或者版本号没有服务器的高，就下载
		{


			blks.blocks_[iter.blockid].id = iter.blockid;//遍历list.json文件里的 id 存到 blks.id(下载时就把一个文件的id  名字存起来)

			blks.blocks_[iter.blockid].files.push_back(iter.path);


			index[iter.blockid] += 1;          //下载一个文件   就让下载的数量+1


			if (id_index[iter.blockid] == index[iter.blockid])//（id:1  id:2）每个id的总数量 == 存到的数量     （数量相等时开始请求下载）
			{
				auto it = blks_.blocks.find(blks.blocks_[iter.blockid].id);//找到要下载文件的id 
				//auto it = std::find_if(blks.blocks_.begin(), blks.blocks_.end(), [&](auto file) {return file == blks.blocks_[iter.blockid].id; });

				//auto it = blks_.blocks.find(blks.blocks_.begin(), blks.blocks_.end(), blks.blocks_[iter.blockid].id);//找到要下载文件的id 
				if (it == blks_.blocks.end())//没有找到
					continue;
				OutputDebugString(L"转文件io_context");

				
				//pool.enqueue(bind(&down_json_client::down_json_run, this, blks.blocks_[iter.blockid], it->second.server.back().ip, it->second.server.back().port, std::to_string(iter.blockid)));
				
		
				QVariant var;
				var.setValue(blks.blocks_[iter.blockid]);
		
				emit sign_down_block(var, it->second.server.back().ip.data(), it->second.server.back().port.data());
				//down_json_run(blks.blocks_[iter.blockid], it->second.server.back().ip, it->second.server.back().port, std::to_string(iter.blockid));
			}
		}
	}

}

void down_json_client::send_id_port(/*const std::string id_port*/std::size_t id, std::string port)//发送成为服务器的id ip port 
{
	
	//
	//std::size_t id_port_len = id_port.size();//id ip port字符串大小                       
	//id_port_buf.resize(sizeof(size_t) + id_port_len);//给id_port_buf分配sizeof(size_t) + id_port_len的长度
	//std::memcpy(id_port_buf.data(), &id_port_len, sizeof(size_t));
	//sprintf(&id_port_buf[sizeof(size_t)], "%s", id_port.c_str());//把文件名赋给&Id_IP_Port_buf[10]
	
	id_port_request req;
	req.body_.id_ = id;
	req.body_.set_port(port);
	
	this->async_write(std::move(req), [this](std::error_code ec, std::size_t)
		{
			std::cout << ec << std::endl;
			if (!ec)
			{
				
			}
		}); 
	//this->async_write(std::move(req), [this](std::error_code ec, std::size_t)
	/*this->async_write(id_port_buf, [this, id_port_len, id_port](std::error_code ec, std::size_t)
		{
			std::cout << ec << std::endl;
			if (!ec)
			{
				std::cout << "发送给服务器  id ip port:  " << id_port_buf << std::endl;
			}
		});*/
}
