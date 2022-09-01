#include "down_block_client.h"
#include "file_server.h"
#include "request.hpp"
#include "response.hpp"
#include <filesystem>

filestruct::wget_c_file_info down_block_client::wcfi_copy ;
filestruct::wget_c_file down_block_client::wcf;
std::mutex down_block_client::write_mtx_;
filestruct::block down_block_client::blk_copy;
down_block_client* down_block_client::client_=nullptr;
std::unordered_map<std::size_t, std::vector<std::string>> down_block_client::total_id_files_num;



void down_block_client::send_filename()
{
	blk_copy = blk;
	

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
		std::filesystem::path{ file_path.substr(0, found) }.parent_path();
		
		std::filesystem::create_directories(file_path.substr(0, found), ec);
		std::cout << "文件夹创建成功\n";
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
		std::size_t recive_len = resp.header_.length_;

		read_name = resp.header_.name_;
		id_num = resp.body_.id_;
		/*std::string*/ file_path_ = downfile_path.path + "\\" + read_name;
		auto total_num = resp.header_.totoal_;
		std::string text_ = resp.body_.text_;

		map_.emplace(read_name, total_num);
		std::ofstream file(file_path_.data(), std::ios::out | std::ios::binary | std::ios::app);


		for (auto iter = map_.begin(); iter != map_.end(); iter++)
		{
			if (iter->first == read_name)
			{

				file.write(text_.data(), recive_len);
				++count;

				if (iter->second == count)
				{

					file.close();
				
					save_location(file_path_, read_name, id_num);
				
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
	wcfi = parse_wget_c_file_json("wget_c_file1.json");


	for (auto& iter : wcfi.wget_c_file_list)
	{
		wcf.wget_name = iter.wget_name;
		wcf.offset = iter.offset;
	
		wcfi_copy.wget_c_file_list.push_back(wcf);
	}
	save_location_connect_error(file_path_, read_name);

	return 0;
}

void down_block_client::save_location_connect_error(const std::string& name,const std::string& no_path_name)
{

	ifstream id_File(name, ios::binary);
	id_File.seekg(0, ios_base::end);
	size_t file_size = id_File.tellg();//文本的大小
	id_File.seekg(0, ios_base::beg);


	wcf.wget_name = no_path_name;
	wcf.offset = file_size;

	write_mtx_.lock();


	wcfi_copy.wget_c_file_list.push_back(wcf);

	write_mtx_.unlock();

	parse_client_list_json("list.json");
	
	for (auto& iter : files_inclient.file_list)
	{

		auto it_client = std::find_if(wcfi_copy.wget_c_file_list.begin(), wcfi_copy.wget_c_file_list.end(), [&](auto file) {return file.wget_name == iter.path; });
		if (it_client == wcfi_copy.wget_c_file_list.end())
		{
			if (iter.path.empty())
				continue;

			wcf.wget_name = iter.path;
			wcf.offset = 0;
		
			wcfi_copy.wget_c_file_list.push_back(wcf);
		}
	}
	
	
	save_wget_c_file_json(wcfi_copy, "wget_c_file.json");
	return ;
}

//断开再连接时     wcfi 清空  断开连接时，保存到一个文件中 ，连接时，先读这个文件   再把这个保存到别的文件中
//保存到wget_c_file文件中 下载完成的文件名  和偏移量
//加锁（需要把这个成员函数设置成全局函数）否则加锁不会成功（成员函数设置成static  成员变量也需要设置成static）
void down_block_client::save_location(const string& name, const string& no_path_add_name,std::size_t id_num)
{
	/*static */std::unordered_map<std::size_t, std::vector<std::string>> id_to_the_files;

	ifstream id_File(name, ios::binary);
	id_File.seekg(0, ios_base::end);
	size_t file_size = id_File.tellg();//文本的大小
	id_File.seekg(0, ios_base::beg);


	wcf.wget_name = no_path_add_name;
	wcf.offset = file_size;

	write_mtx_.lock();

	wcfi_copy.wget_c_file_list.push_back(wcf);

	write_mtx_.unlock();



	id_to_the_files[id_num].push_back(no_path_add_name);        

	if (total_id_files_num[id_num].size() == id_to_the_files[id_num].size())
	{
		client_to_server(downfile_path.port);

		QString get_port = QString::fromStdString(downfile_path.port);

		
		emit client_->signal_get_id_port_externl(id_num, get_port);
		
		OutputDebugString(L"id 块下载完成  客户端转服务器");
	}

	save_wget_c_file_json(wcfi_copy, "wget_c_file1.json");

}

void down_block_client::save_wget_c_file_json(filestruct::wget_c_file_info wcfi,  string name)
{
	string text = RapidjsonToString(wcfi.serializeToJSON());
	json_formatting(text);


	auto file = fopen(name.c_str(), "wb");

	const char* t = text.c_str();
	size_t length = text.length();
	fwrite(t, length, 1, file);
	fflush(file);
	fclose(file);

}

void down_block_client::client_to_server(string profile_port)
{
	std::thread t(std::bind(&down_block_client::server, profile_port));
	
	t.detach();
}

void down_block_client::server(const std::string& server_port)
{
	int port = atoi(server_port.c_str());
	asio::io_context io_context;
	asio::ip::tcp::endpoint _endpoint(asio::ip::tcp::v4(), port);
	auto fs = std::make_shared<file_server>(io_context, _endpoint);

	io_context.run();
}


