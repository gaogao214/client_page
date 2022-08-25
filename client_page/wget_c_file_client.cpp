#include "wget_c_file_client.h"
#include <iostream>

void wget_c_file_client::do_send_wget_file_name()
{
	size_t list_name_len = wget_c_name.size();//计算出断点续传.json名字的大小
	send_name.resize(sizeof(size_t) + list_name_len);
	//sprintf(&send_name[0], "%d", static_cast<int>(list_name_len));
	memcpy(send_name.data(), &list_name_len, sizeof(size_t));
	sprintf(&send_name[sizeof(size_t)], "%s", wget_c_name.c_str());

	this->async_write(send_name, [this](std::error_code ec, std::size_t sz)
		{
			if (!ec)
			{
				OutputDebugString(L"s 断点续传文件发送成功");

				do_send_wget_file_name_offset();//发送文件的内容
			}
		});
	
}

/*发送断点续传的内容*/
void wget_c_file_client::do_send_wget_file_name_offset()
{

	size_t fsize = send_file_len(wget_c_name);
	std::string list_buf = send_file_context(wget_c_name);
	wget_text.resize(sizeof(size_t) + fsize);
	memcpy(wget_text.data(), &fsize, sizeof(size_t));
	sprintf(&wget_text[sizeof(size_t)], "%s", list_buf.c_str());

	this->async_write(wget_text,[this](std::error_code ec, std::size_t)
	{
		if (!ec)
		{
			OutputDebugString(L"s 断点续传文件内容成功");
			//emit sign_wget_c_file_text_log(u8"发送断点续传文件内容成功\n");
			//do_recive_wget_file();
		}
	});
	 
}

void wget_c_file_client::do_recive_wget_file(std::size_t len)
{
	std::string recive_wget_buf(buffer_.data(),len);

	std::string name = recive_wget_buf.substr(0,8);
	std::string total_num = recive_wget_buf.substr(8, 8);
	std::string offset_ = recive_wget_buf.substr(16, 8);
	std::string text = recive_wget_buf.substr(24);

	//OutputDebugString(L"s 文件接收成功");

	std::size_t tatal_num_{};
	std::size_t offset_num_{};
	std::memcpy(&tatal_num_,total_num.data(),sizeof(size_t));   //总序号
	std::memcpy(&offset_num_, offset_.data(), sizeof(size_t));   //总序号


	name_num_map_.emplace(name, tatal_num_);

	std::string file_name = downfile_path.path + "\\" + name;


	std::ofstream file(file_name.data(), std::ios::out | std::ios::binary | std::ios::app);
	
	for (auto iter = name_num_map_.begin(); iter != name_num_map_.end(); iter++)
	{
		if (iter->first == name)
		{
			std::size_t dowm_size = len - 8 - 8 - 8;

			file.seekp(offset_num_, std::ios::beg);

			file.write(text.c_str(), dowm_size);
			++count;

			if (iter->second == count)
			{
				file.close();

			}
		}

	}
	

	std::cout << file_name << "文件接收成功\n";
}

int wget_c_file_client::read_handle(std::size_t bytes_transferred)
{

	do_recive_wget_file(bytes_transferred);
	return 0;

}

int wget_c_file_client::read_error()
{

	return 0;
}
