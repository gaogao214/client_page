#pragma once

#include <asio.hpp>
#include <fstream>
#include "common.h"
#include "rapidjson/filereadstream.h"
#include "GxJsonUtility.h"
#include <memory>
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include "ThreadPool.h"


using namespace asio::ip;

namespace filestruct {
	struct list_json {                 //list.json
		std::string path;
		uint32_t version;
		uint32_t blockid;
		GX_JSON(path, version, blockid);
	};
	
	struct files_info                  //list.json
	{
		std::vector<list_json> file_list;
		GX_JSON(file_list);
	};

	struct id_json {                   //id.json
		std::string ip;
		std::string port;
		GX_JSON(ip, port)
	};
	struct block_info {                //id.json
		int id;
		std::vector<id_json> server;
		GX_JSON(id, server)
	};
	struct blocks {                    //id.json
		std::unordered_map<int, block_info> blocks;
		//vector<block_info> blocks;
		GX_JSON(blocks);
	};
	struct profile {                  //�����ļ�
		std::string path;
		std::string port;
		GX_JSON(path, port);
	};

	struct profile_info               //�����ļ�
	{
		std::vector<profile> file_list;
		GX_JSON(file_list);
	};

	struct block                      //��һ����id���ļ���
	{
		int id;
		std::vector<std::string> files;
	};

	struct blocks_for_download
	{
		std::unordered_map<int, block> blocks_;
	};
	struct wget_c_file                 //�ϵ������ļ�
	{
		std::string wget_name;
		uint32_t offset;
		GX_JSON(wget_name,offset);
	};
	struct wget_c_file_info
	{
		std::vector<wget_c_file> wget_c_file_list;
		GX_JSON(wget_c_file_list);
	};
	

}

class client_page;
class down_json;
//
//extern down_json* g_down_json;
extern  filestruct::profile downfile_path;      //�����ļ�
extern  filestruct::files_info files_inclient;	//�����ͻ��˱��ص�json�ı�

class down_json :public std::enable_shared_from_this<down_json>
{
public:
	down_json(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints, client_page* cli);

	down_json(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints);

	//void get_list_from_server_();

	void parse_down_jsonfile(std::string& name);			//��down.json�����ļ�

	void parse_client_list_json(std::string& name);		//��list.json�����ļ�

	void parse_server_list_json(std::string& text_json);//��list.json�����ļ�

	void parse_block_json(std::string& text_json); //��id.json�����ļ�

	void send_id_port(const std::string id_port);	//���ͳ�Ϊ��������id   port

	void down_json_run(filestruct::block Files, std::string& loadip, std::string& loadport, const std::string& comePort);//����

	std::string open_json_file(const std::string& json_name);  //��json�ļ�

	filestruct::wget_c_file_info parse_wget_c_file_json(std::string& name);//��wget_c_file.json �ϵ����������ļ�


	filestruct::files_info files_inserver;		//������������json�ı�

	filestruct::blocks blks_;					//����id.json�ı�
	filestruct::blocks_for_download blks;		//��һ����id���ļ���
	std::string list_json = "list.json";
	std::string id_json = "id.json";
	std::string down_json_name = "down.json";
	std::string wget_c_file = "wget_c_file.json";
	std::string wget_c_file1 = "wget_c_file1.json";
	filestruct::wget_c_file_info wcfi;			//����һ���ṹ��

	void run()
	{
		io_context_.run();
	}

private:

	void do_connect(const asio::ip::tcp::resolver::results_type& endpoints);
	
	void recive_list();							//����list.json�ļ���������
	
	void recive_id();							//����id.json�ļ��� ������

	void isfile_exist( const std::string file_buf,int buf_len);//�ж��ļ����Ƿ����
	
	void down_load();							//��һ���̳߳�

	void save_file(const std::string& name , const std::string& file_buf);//�����ļ�

	asio::io_context& io_context_;
	asio::ip::tcp::socket socket_;

	char list_len[sizeof(size_t)];			//����list.json�ļ����ĳ���
	std::string list_name;						//����llist.json�ļ���
	int id_name_len ;
	int list_name_len ;
	char id_len[sizeof(size_t)];			//����id.json�ļ����ĳ���
	std::string id_name_text;					//����id.json�ļ���
	std::unordered_map<int, int> index;			//���صĴ���
	std::unordered_map<int, int> id_index;		//һ����id  ����
	std::string id_port_buf;						//���͵�id��port�ĳ��Ⱥ�����
	ThreadPool pool;
	volatile int len = 0;
	char str_[1024]="get";
	client_page* cli_ptr_;
};