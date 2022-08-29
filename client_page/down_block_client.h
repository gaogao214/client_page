#pragma once
#include <asio.hpp>
#include <thread>
#include <pthread.h>
#include <unordered_map>
#include <QObject>
#include "basic_client.h"

#include "down_json_client.h"


class down_block_client : public basic_client 
{
	Q_OBJECT
public:
	down_block_client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints, const filestruct::block& block = {})
		: basic_client(io_context, endpoints)
		, blk(block)
	{
		client_ = this;
	
	}

public:

	void send_filename();

	void recive_file_text(size_t recive_len);

protected:

	virtual int read_handle(std::size_t bytes_transferred) override;
	virtual int read_error() override;

public:


	void does_the_folder_exist(const std::string& list_name);//�ж��ļ����Ƿ���ڣ��������򴴽��ļ���

	static void client_to_server(std::string profile_port);    //��һ���̣߳��ͻ���ת���ɷ����

	static void server(const std::string& server_port);      //�ͻ���ת���ɷ����

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
	static void save_location(const std::string& name, const std::string& no_path_add_name,std::size_t id_num);    /*��¼��ͣ����ʱ��  �ļ����Լ�ƫ����  */

	static void save_wget_c_file_json(filestruct::wget_c_file_info wcfi, /*const */std::string name);

	void Breakpoint_location();    /*��¼��ͣ����ʱ��  �ļ����Լ�ƫ����  */

	static void json_formatting(std::string& strtxt);//���ո�ʽд��id.json �ļ�


signals:
	
	void signal_get_id_port_externl(std::size_t get_server_id, QString get_server_port);
	 void signal_pro_bar(int maxvalue, int value);
	 void signal_file_name_(QString file_name_);


private:
	filestruct::block blk;//feige

	filestruct::wget_c_file_info wcfi;  //����һ���ṹ��

	int downloadingIndex = 0;//������ĸ���
	std::string file_name;
	std::string file_path;   //·��+�ļ���
	

	//����   �����   ����
	std::string read_name;     //����    name
	std::string file_path_;   //����·��������   path + name
	std::vector<std::string> vec_text_;
	
	std::map<std::string, std::size_t> map_;
	std::string texts_;
	std::size_t count = 0;
	
	std::string id;
	size_t id_num;
public:

	static std::unordered_map<std::size_t, std::vector<std::string>> id_to_the_files;
	static std::unordered_map<std::size_t, std::vector<std::string>> total_id_files_num;

	static std::mutex write_mtx_;

	static filestruct::wget_c_file_info wcfi_copy;  //����һ���ṹ��
	static filestruct::wget_c_file wcf;
	static filestruct::block blk_copy;

	static down_block_client* client_;

};

