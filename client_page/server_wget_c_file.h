#pragma once
#pragma once
#include "asio.hpp"
#include <iostream>
#include <fstream>
#include "common.h"
#include "GxJsonUtility.h"
#include "rapidjson/filereadstream.h"
#include <deque>

using namespace std;
using namespace asio::ip;

class wget_load_file :public std::enable_shared_from_this<wget_load_file>
{

public:
	wget_load_file(tcp::socket socket);

	void start();

	void recive_wget_c_file_name();   //���նϵ������ļ�������

	void recive_wget_c_file(const string& file);  //���նϵ�����������

	void do_wget_c_file(const string& file_name);   //����json�ļ�

	void send_file();             //�鿴�ϵ�ʱ  ��Щ�ļ���Ҫ�����ϴ�

	filestruct::wget_c_file_info  wget_c_file_profile(string& name);//�򿪶ϵ������ļ�  ����json�ļ�

	void  down_json_profile(string& name);//�������ļ������ҵ������ļ��е�·��,�鿴·���µ��ļ����ļ���   ����json�ļ�
	
	std::string open_json_file(const std::string& json_name);//��ָ�����Ƶ�json�ı�
	
private:

	inline size_t get_file_len(const std::string& filename)
	{
		size_t fsize = 0;
		ifstream infile(filename.c_str());
		if (!infile.is_open())
		{
			cout << "�ļ�����û�д��ļ� >:" << filename << endl;
			return 0;
		}
		infile.seekg(0, ios_base::end);
		fsize = infile.tellg();//list.json�ı��Ĵ�С
		infile.close();

		return fsize;
	}

	inline string/*char**/ get_file_context(const std::string& filename, int remaining_total, int offset/*,int count,int read_bytes=65536*/)//�ı�������
	{

		std::string content{};
		std::string tmp{};
		std::fill(content.begin(), content.end(), 0);//���

		fstream ifs(filename, std::ios::in | std::ios::binary);
		if (!ifs.is_open())
			return {};

		//ifs.seekg(offset+(count*65536), ios::cur);
		ifs.seekg(offset, ios::cur);

		while (std::getline(ifs, tmp))
		{
			content.append(tmp);
		}

		return content;
	}

	void write(const std::string& msg);       //����ļ�ʱ����Ҫ�ϴ����ļ����ݴ��������   
	void do_write();                          //�ϴ�����

	std::deque<std::string> write_msgs_;
	std::mutex write_mtx_;
	asio::ip::tcp::socket socket_;
	char refile_name[1024] = { 0 };//�������ֵĳ���
	size_t filelen = 0;         //���ֵĳ���
	char refile_file_len[sizeof(size_t)] = { 0 };//�������ֵĳ���
	string file_path_name;//·��+�ļ���
	std::string send_wget_name_and_offset_len{};
	std::string count_file_buf;//�������ַ�
	size_t file_size = 0;//�ļ����ܴ�С
	string wget_file_string;//���Ͷϵ��ļ��е��ļ����µ�����
	string wget_c_file_text; //�����ļ�������
	int buf_len;   //�����ļ����ݵĳ���
	string wget_name;        //�ϵ������е��ļ���
	int wget_offset;		//�ϵ������е�ƫ����
	string name_and_offset_remaining;       /* ����  ƫ����  �������ݵĳ��� �Լ����µ�����ƴ��һ�� */
	size_t remaining_total;                 //�������µĳ���
	filestruct::wget_c_file_info wcfi;			//�ṹ��ʵ����
	std::string down_json_name="down.json";


};

class wget_c_file_accept
{
public:
	wget_c_file_accept(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint)
		:acceptor_(io_context, endpoint)
	{
		do_accept();
	}

	void do_accept()
	{
		acceptor_.async_accept(
			[this](std::error_code ec, asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					cout << "����˶˿� 12313 �Ϳͻ��˶˿� 12313 ���ӳɹ�\n";
					auto fs = std::make_shared<wget_load_file>(std::move(socket));
					fs->start();
					sessions.push_back(fs);
				}
				else
				{
					cout << "����˶˿� 12313 �Ϳͻ��˶˿� 12313 ����ʧ��\n";
				}
				do_accept();
			});
	}

private:
	std::vector<std::shared_ptr<wget_load_file>> sessions;
	asio::ip::tcp::acceptor acceptor_;

};
