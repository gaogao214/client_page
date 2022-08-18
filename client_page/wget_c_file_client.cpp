#include "wget_c_file_client.h"
#include <iostream>

void wget_c_file_client::do_send_wget_file_name()
{
	size_t list_name_len = wget_c_name.size();//������ϵ�����.json���ֵĴ�С
	send_name.resize(sizeof(size_t) + list_name_len);
	//sprintf(&send_name[0], "%d", static_cast<int>(list_name_len));
	memcpy(send_name.data(), &list_name_len, sizeof(size_t));
	sprintf(&send_name[sizeof(size_t)], "%s", wget_c_name.c_str());

	this->async_write(send_name, [this](std::error_code ec, std::size_t sz)
		{
			if (!ec)
			{
				OutputDebugString(L"s �ϵ������ļ����ͳɹ�");

				do_send_wget_file_name_offset();//�����ļ�������
			}
		});
	
}

/*���Ͷϵ�����������*/
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
			OutputDebugString(L"s �ϵ������ļ����ݳɹ�");
			//emit sign_wget_c_file_text_log(u8"���Ͷϵ������ļ����ݳɹ�\n");
			//do_recive_wget_file();
		}
	});
	 
}

void wget_c_file_client::do_recive_wget_file()
{
	auto pos = recive_wget_name.find_first_of("!");
	auto remaining_total = recive_wget_name.substr(0, pos);		//���յ��ַ����ܳ���
	std::cout << "total >:" << remaining_total << std::endl;

	auto str = recive_wget_name.substr(pos + 1);	//�ϵ�ʱ �ļ��� ƫ����  �Լ����µ�����
	auto pos1 = str.find_first_of(',');
	auto name = str.substr(0, pos1);
	std::cout << "name >: " << name << std::endl;  //ƫ����
	//emit sign_wget_c_file_name(name.c_str());
	auto  str1 = str.substr(pos1 + 1); //���µ�����
	auto pos2 = str1.find_first_of('*');
	auto offset = str1.substr(0, pos2);
	wget_text = str1.substr(pos2 + 1);

	OutputDebugString(L"s �ļ����ճɹ�");



	std::string file_name = downfile_path.path + "\\" + name;

	int offset_len = atoi(offset.data());
	int total = atoi(remaining_total.data());
	std::ofstream file(file_name.data(), std::ios::out | std::ios::binary | std::ios::app);
	file.seekp(offset_len, std::ios::beg);

	file.write(wget_text.c_str(), total);

	file.close();
	std::cout << file_name << "�ļ����ճɹ�\n";
}

int wget_c_file_client::read_handle(std::size_t bytes_transferred)
{

	do_recive_wget_file();
	return 0;

}
