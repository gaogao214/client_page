#include "down_block_client.h"
#include "client_to_the_server.h"
#include "request.hpp"
#include "response.hpp"
#include <filesystem>


std::mutex down_block_client::write_mtx_;
filestruct::block down_block_client::blk_copy;
down_block_client* down_block_client::client_=nullptr;
std::unordered_map<std::size_t, std::vector<std::string>> down_block_client::total_id_files_num;
std::unordered_map<std::size_t, std::vector<std::string>> down_block_client::id_to_the_files;
std::string down_block_client::str_file;

void down_block_client::send_filename()
{
	blk_copy = blk;

	std::string s = std::to_string(GetCurrentThreadId());
	OutputDebugStringA(s.data());

	for (auto iter : blk.files)
	{	
		auto name = iter;

		total_id_files_num[blk.id].push_back(name);

		if (name.empty())
			continue;
		
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(200ms);

		id_name_request req;
		req.body_.id_=blk.id;
		req.body_.set_name(name);

	this->async_write(std::move(req),[name, this](std::error_code ec, std::size_t)
		{
			if (!ec)
			{
				does_the_folder_exist(name);
				OutputDebugStringA(name.data());
				OutputDebugString(L"\n");

			}
		});
	}
}



void down_block_client::does_the_folder_exist(const std::string& list_name)//判断文件夹是否存在，不存在则创建文件夹
{

	std::string file_path;   //路径+文件名

	file_path = downfile_path.path + "\\" + list_name;
	std::size_t found = file_path.find_last_of("\\");

	std::error_code ec;
	if (!std::filesystem::exists(file_path.substr(0, found)))//不存在创建
	{
		std::filesystem::create_directories(file_path.substr(0, found), ec);
	}

}


void down_block_client::recive_file_text(uint32_t id)
{
	std::size_t id_num;
	switch (id)
	{
	case response_number::id_text_response_:

		id_text_response resp;
		std::memset(resp.header_.name_, 0, 32);//清空内存

		resp.parse_bytes(buffer_.data());
		recive_len = resp.header_.length_;

		read_name = resp.header_.name_;
		id_num = resp.body_.id_;
		file_path = downfile_path.path + "\\" + read_name;
		auto total_num = resp.header_.totoal_;
		std::string text_ = resp.body_.text_;

		map_.emplace(read_name, total_num);
		std::ofstream file(file_path.data(), std::ios::out | std::ios::binary | std::ios::app);


		for (auto iter = map_.begin(); iter != map_.end(); iter++)
		{
			if (iter->first == read_name)
			{

				file.write(text_.data(), recive_len);
				++count;

				if (iter->second == count)
				{

					file.close();
				
					save_location(file_path, read_name, id_num);
				
					count = 0;
				}
			}
		}

		break;
	}
}

int down_block_client::read_handle(uint32_t id)
{
	recive_file_text(id);

	return 0;
}

int down_block_client::read_error()
{
	
	save_location_connect_error(file_path, read_name);

	emit client_->signal_wget_down_file();
	return 0;
}

void down_block_client::save_location_connect_error(const std::string& name,const std::string& no_path_name)
{
	filestruct::wget_c_file_info wcfi_copy;
	filestruct::wget_c_file wcf;

	std::ifstream id_file(name, std::ios::binary);
	id_file.seekg(0, std::ios_base::end);
	size_t file_size = id_file.tellg();
	id_file.seekg(0, std::ios_base::beg);

	if (file_size < recive_len) 
	{
		wcf.wget_name = no_path_name;
		wcf.offset = file_size;

		write_mtx_.lock();


		wcfi_copy.wget_c_file_list.push_back(wcf);

		write_mtx_.unlock();
	}

	
	for (auto& iter : files_inserver.file_list)
	{

		auto it_client = std::find_if(wcfi.wget_c_file_list.begin(), wcfi.wget_c_file_list.end(), [&](auto file) {return file.wget_name == iter.path; });
		if (it_client == wcfi.wget_c_file_list.end())
		{
			if (iter.path.empty())
				continue;

			wcf.wget_name = iter.path;
			wcf.offset = 0;
			wcfi_copy.wget_c_file_list.push_back(wcf);
			
		}
	}

	str_file = "wget_c_file" + std::to_string(GetCurrentThreadId()) + ".json";

	save_wget_c_file_json(wcfi_copy, str_file);


	return ;
}

//断开再连接时     wcfi 清空  断开连接时，保存到一个文件中 ，连接时，先读这个文件   再把这个保存到别的文件中
//保存到wget_c_file文件中 下载完成的文件名  和偏移量
//加锁（需要把这个成员函数设置成全局函数）否则加锁不会成功（成员函数设置成static  成员变量也需要设置成static）
void down_block_client::save_location(const std::string& name, const std::string& no_path_add_name,std::size_t id_num)
{

	filestruct::wget_c_file wcf;
	filestruct::wget_c_file_info wcfi_;
	std::ifstream id_file(name, std::ios::binary);
	id_file.seekg(0, std::ios_base::end);
	size_t file_size = id_file.tellg();//文本的大小
	id_file.seekg(0, std::ios_base::beg);
	if (no_path_add_name.empty())
	{
		return;
	}
	
	
	wcf.wget_name = no_path_add_name;
	wcf.offset = file_size;
	wcf.id = id_num;
	//write_mtx_.lock();

	wcfi_.wget_c_file_list.push_back(wcf);

	//write_mtx_.unlock();
	str_file = "wget_c_file" + std::to_string(GetCurrentThreadId()) + ".json";

	save_wget_c_file_json(wcfi_, str_file);



	id_to_the_files[id_num].push_back(no_path_add_name);        

	if (total_id_files_num[id_num].size() == id_to_the_files[id_num].size())
	{
		
		client_to_server(downfile_path.port);

		QString get_port = QString::fromStdString(downfile_path.port);
		Sleep(2);
		
		emit client_->signal_get_id_port_externl(id_num, get_port);
		
		OutputDebugString(L"id 块下载完成  客户端转服务器");
	}


}

void down_block_client::save_wget_c_file_json(filestruct::wget_c_file_info wcfi,  std::string name)
{
	std::string text = RapidjsonToString(wcfi.serializeToJSON());
	json_formatting(text);

	save_file(name.c_str(),text.c_str() );

	return;
}

void down_block_client::client_to_server(std::string profile_port)
{
	std::thread t(std::bind(&down_block_client::server, profile_port));
	
	t.detach();
}

void down_block_client::server(const std::string& server_port)
{
	int port = atoi(server_port.c_str());
	asio::io_context io_context;
	asio::ip::tcp::endpoint _endpoint(asio::ip::tcp::v4(), port);
	auto fs = std::make_shared<client_to_the_server>(io_context, _endpoint);
	fs->run();
	
}


