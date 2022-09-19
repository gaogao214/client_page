#include "down_block_client.h"
#include "client_to_the_server.h"
#include "request.hpp"
#include "response.hpp"
#include <filesystem>


void down_block_client::send_filename()
{
	for (auto iter : blk.files)
	{	

		total_id_files_num[blk.id].push_back(iter);

		if (iter.empty())
			continue;
		
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(200ms);

		id_name_request req;
		req.body_.id_=blk.id;
		req.body_.set_name(iter);

		this->async_write(std::move(req),[iter, this](std::error_code ec, std::size_t)
			{
				if (!ec)
				{
					does_the_folder_exist(iter);
					OutputDebugStringA(iter.data());
					OutputDebugString(L"\n");

				}
			});
	}
}

void down_block_client::does_the_folder_exist(const std::string& list_name)
{

	std::string file_path;   

	file_path = downfile_path.path + "\\" + list_name;
	std::size_t found = file_path.find_last_of("\\");

	std::error_code ec;
	if (!std::filesystem::exists(file_path.substr(0, found)))
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

		std::memset(resp.header_.name_, 0, 32);

		resp.parse_bytes(buffer_.data());

		recive_len = resp.header_.length_;
		read_name = resp.header_.name_;
		emit signal_file_name_(resp.header_.name_);

		id_num = resp.body_.id_;

		file_path = downfile_path.path + "\\" + resp.header_.name_;

		auto total_num = resp.header_.totoal_sequence_;

		std::string text_ = resp.body_.text_;

		map_.emplace(read_name, total_num);

		std::ofstream file(file_path.data(), std::ios::out | std::ios::binary | std::ios::app);


		for (auto iter = map_.begin(); iter != map_.end(); iter++)
		{
			if (iter->first == read_name)
			{

				file.write(text_.data(), recive_len);
				++count;

				
				if (resp.header_.totoal_length_ <= 0)
					return;
				
				emit signal_pro_bar(resp.header_.totoal_length_, recive_len * (count));

			

				if (iter->second == count)
				{

					file.close();
				
					save_location(file_path, read_name, id_num);
					
					std::string str = resp.header_.name_ + std::string(u8"接收成功\n");
					QString qstr = QString::fromStdString(str);

					emit signal_text_log(qstr);
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

	QString wget_file_name = QString::fromStdString(str_file) ;

	emit signal_wget_down_file(wget_file_name);

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
		
		wcfi_copy.wget_c_file_list.push_back(wcf);
	}

	for (auto& iter : files_inserver.file_list)
	{
		if (number_ != iter.blockid)
			continue;

		auto it_client = std::find_if(wcfi.wget_c_file_list.begin(), wcfi.wget_c_file_list.end(), [&](auto file) {return file.wget_name == iter.path; });

		if (it_client == wcfi.wget_c_file_list.end())
		{
			if (iter.path.empty())
				continue;

			wcf.wget_name = iter.path;
			wcf.offset = 0;
			wcf.id = iter.blockid;

			wcfi_copy.wget_c_file_list.push_back(wcf);

		}
	}

	str_file = "wget_c_file_" + std::to_string(number_) + ".json";

	save_wget_c_file_json(wcfi_copy, str_file);

	return ;
}

void down_block_client::save_location(const std::string& name, const std::string& no_path_add_name,std::size_t id_num)
{

	filestruct::wget_c_file wcf;

	std::ifstream id_file(name, std::ios::binary);

	id_file.seekg(0, std::ios_base::end);
	std::size_t file_size = id_file.tellg();
	id_file.seekg(0, std::ios_base::beg);

	if (no_path_add_name.empty())
		return;
	
	wcf.wget_name = no_path_add_name;
	wcf.offset = file_size;
	wcf.id = id_num;

	wcfi.wget_c_file_list.push_back(wcf);

	id_to_the_files[id_num].push_back(no_path_add_name);        

	if (total_id_files_num[id_num].size() == id_to_the_files[id_num].size())
	{

		client_to_server(downfile_path.port);

		QString get_port = QString::fromStdString(downfile_path.port);
		
		emit signal_get_id_port_externl(id_num, get_port);
		
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
	std::thread t(std::bind(&down_block_client::server,this, profile_port));
	
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


