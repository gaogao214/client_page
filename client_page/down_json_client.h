#pragma once
#include "basic_client.h"
#include <memory>
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <string>
#include <algorithm>
#include <fstream>
#include "ThreadPool.h"
#include "file_struct.h"
#include "io_context_pool.h"
class down_block_client;
extern  filestruct::profile downfile_path;      //�����ļ�
extern  filestruct::files_info files_inclient;	//�����ͻ��˱��ص�json�ı�


Q_DECLARE_METATYPE(filestruct::block)
class down_json_client 
	: public basic_client
{
	Q_OBJECT
public:
	down_json_client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints)
		: basic_client(io_context, endpoints)
		, pool(2)
		//, io_pool_block_(2)
	{
		parse_down_jsonfile(down_json_name);
	}

public:

	void receive_buffer(std::size_t length);

	/*void set_funtion(std::function<void()>&& func)
	{
		func_.swap(func);
	}*/

protected:
	virtual int read_handle(std::size_t bytes_transferred) override;

private:
	void  parse_server_list_json(std::string text_json);//��list_json   json�ļ�  ����json�ļ�
	

	void  parse_down_jsonfile(std::string name);//�������ļ������ҵ������ļ��е�·��,�鿴·���µ��ļ����ļ���   ����json�ļ�


	void  parse_block_json(std::string text_json);//��list_json   json�ļ�  ����json�ļ�


	void parse_client_list_json(std::string name);//��list_json   json�ļ�  ����json�ļ�
	
	void isfile_exist(const std::string file_buf, int buf_len);//�ж�list.json�ļ��Ƿ����,���ھͽ���json�ı���server��json���бȽϣ������ھͱ����ļ�

	std::string open_json_file(const std::string& json_name);//��ָ�����Ƶ�json�ı�

	void save_file(const std::string& name, const std::string& file_buf);//��������

	void down_json_run(filestruct::block Files, std::string loadip, std::string loadport, const std::string& down_id);//���������ļ��Ķ˿�

	void down_load();//����������̳߳������������������

	void send_id_port(const std::string id_port);//���ͳ�Ϊ��������id ip port 

signals:

	void sign_pro_bar(int maxvalue_, int value_);
	void sign_file_name(QString file_name);
	void sign_text_log(QString log_);
	void sign_down_block(QVariant var,QString loadip, QString loadport);



//
//public:
//	void start_io_pool()
//	{
//		pool_thread_block_ptr_.reset(new std::thread([this]
//			{
//				this->io_pool_block_.run();
//			}));
//	}
//
//
//public:
//	io_context_pool io_pool_block_;
//private:
//	std::shared_ptr<std::thread> pool_thread_block_ptr_;
private:
	std::vector<std::shared_ptr<down_block_client>> down_block_list_;



public:
	std::string wget_c_file = "wget_c_file.json";
	std::string wget_c_file1 = "wget_c_file1.json";

private:
	std::string list_name;						//����llist.json�ļ���
	std::string id_name_text;					//����id.json�ļ���

	filestruct::files_info files_inserver;		//������������json�ı�

	filestruct::blocks blks_;					//����id.json�ı�
	filestruct::blocks_for_download blks;		//��һ����id���ļ���


	std::string list_json = "list.json";
	std::string id_json = "id.json";
	std::string down_json_name = "down.json";

	volatile int len = 0;
	std::unordered_map<int, int> id_index;		//һ����id  ����
	std::unordered_map<int, int> index;			//���صĴ���

	std::string id_port_buf;						//���͵�id��port�ĳ��Ⱥ�����
	ThreadPool pool;



	std::function<void()> func_;
};