#pragma once
#include <asio.hpp>
#include "basic_client.h"

#include "down_json_client.h"


class down_block_client : public basic_client 
{
public:
	down_block_client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints, const filestruct::block& block = {})
		: basic_client(io_context, endpoints)
		, blk(block)
	{
		
	}

public:

	void send_filename();

	void recive_file_text(size_t recive_len);

protected:

	virtual int read_handle(std::size_t bytes_transferred) override;
	virtual int read_error() override;

public:


	void does_the_folder_exist(const std::string& list_name);//�ж��ļ����Ƿ���ڣ��������򴴽��ļ���

	void client_to_server(std::string profile_port);    //��һ���̣߳��ͻ���ת���ɷ����

	void server(const std::string& server_port);      //�ͻ���ת���ɷ����

	filestruct::wget_c_file_info  parse_wget_c_file_json(const std::string& name)//�򿪶ϵ������ļ�  ����json�ļ�
	{
		std::string readbuffer = open_json_file(name);

		wcfi.deserializeFromJSON(readbuffer.c_str());

		return wcfi;
	}


	std::string open_json_file(const std::string& json_name)//��ָ�����Ƶ�json�ı�
	{
		std::string content{};
		std::string tmp{};
		std::fill(content.begin(), content.end(), 0);//���

		std::fstream ifs(json_name, std::ios::in | std::ios::binary);
		if (!ifs.is_open())
			return {};

		while (std::getline(ifs, tmp))
		{
			content.append(tmp);
		}

		return content;

	}
	void save_location(const std::string& name, const std::string& no_path_add_name);    /*��¼��ͣ����ʱ��  �ļ����Լ�ƫ����  */

	void save_wget_c_file_json(filestruct::wget_c_file_info wcfi, const std::string& name);

	void Breakpoint_location();    /*��¼��ͣ����ʱ��  �ļ����Լ�ƫ����  */

	void gsh(std::string& strtxt);//���ո�ʽд��id.json �ļ�


private:
	filestruct::block blk;//feige
	filestruct::wget_c_file wcf;

	filestruct::wget_c_file_info wcfi;  //����һ���ṹ��

	filestruct::wget_c_file_info wcfi_copy;  //����һ���ṹ��

	//down_json_client dj;       //����һ��down_json��

	int downloadingIndex = 0;//������ĸ���
	std::string file_name;
	std::string file_path;   //·��+�ļ���
	std::string name;




	//����   �����   ����
	std::string read_name;     //����    name
	std::string file_path_;   //����·��������   path + name
	std::vector<std::string> vec_text_;
	//std::map<std::string, std::vector<std::string>> num_text_;
	//std::map<std::string, std::map<std::string, std::vector<std::string>>> map_;
	std::map<std::string, std::size_t> map_;
	std::string texts_;
	std::size_t count = 0;
	//std::array<char, 5> arr{ 'a','a','a','a','a' };



	std::deque<std::string> write_msgs_;
	//std::deque<char *> write_msgs_;
	//std::deque<std::array<char, 8192 + 1024>> write_msgs_;

	std::mutex write_mtx_;

};

