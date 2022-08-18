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

void wget_c_file_client::do_recive_wget_file()
{
	auto pos = recive_wget_name.find_first_of("!");
	auto remaining_total = recive_wget_name.substr(0, pos);		//接收的字符串总长度
	std::cout << "total >:" << remaining_total << std::endl;

	auto str = recive_wget_name.substr(pos + 1);	//断点时 文件名 偏移量  以及余下的内容
	auto pos1 = str.find_first_of(',');
	auto name = str.substr(0, pos1);
	std::cout << "name >: " << name << std::endl;  //偏移量
	//emit sign_wget_c_file_name(name.c_str());
	auto  str1 = str.substr(pos1 + 1); //余下的内容
	auto pos2 = str1.find_first_of('*');
	auto offset = str1.substr(0, pos2);
	wget_text = str1.substr(pos2 + 1);

	OutputDebugString(L"s 文件接收成功");



	std::string file_name = downfile_path.path + "\\" + name;

	int offset_len = atoi(offset.data());
	int total = atoi(remaining_total.data());
	std::ofstream file(file_name.data(), std::ios::out | std::ios::binary | std::ios::app);
	file.seekp(offset_len, std::ios::beg);

	file.write(wget_text.c_str(), total);

	file.close();
	std::cout << file_name << "文件接收成功\n";
}

int wget_c_file_client::read_handle(std::size_t bytes_transferred)
{

	do_recive_wget_file();
	return 0;

}
