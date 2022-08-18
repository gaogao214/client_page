#pragma once
#include <asio.hpp>
#include <QObject>
#include <QVariant>
#include "common.h"
#include "GxJsonUtility.h"
#include "rapidjson/filereadstream.h"


using namespace asio::ip;
using namespace std;
//
//namespace filestruct {
//	struct blocks_for_download;
//	struct block;
//}

class down_block:public QObject/*:public enable_shared_from_this<down_block>*/
{
	Q_OBJECT
public:
	down_block(asio::io_context& io_context, asio::ip::tcp::resolver::results_type& endpoints,/*QVariant var*/ filestruct::block& Files/*, client_page* cli_ptr*/);

	void do_connect( asio::ip::tcp::resolver::results_type& endpoints);

	void do_send_filename();//发送名字

	void does_the_folder_exist(const string& list_name);//判断文件夹是否存在，不存在则创建文件夹

	void do_recive_file_text(const string& fname,int recive_len,const string& no_path_added_name);//接收文件内容

	void gsh(std::string& strtxt);//按照格式写入id.json 文件

	void client_to_server(string profile_port);    //开一个线程，客户端转换成服务端

	void server(const std::string& server_port);      //客户端转换成服务端

	void save_location(const string& name,const string& no_path_add_name);    /*记录暂停下载时的  文件名以及偏移量  */

	void save_wget_c_file_json(filestruct::wget_c_file_info wcfi,const string& name);

	void Breakpoint_location();    /*记录暂停下载时的  文件名以及偏移量  */

	//void save_file(const string& fname, const std::string& file_buf);

	void run()
	{
		io_context_.run();
	}
	

signals:

	void signal_pro_bar(int maxvalue,int value);
	void signal_file_name_(QString file_name_);
	


public:
	string id_ip_port_;

private:
	//filestruct::block blk;//2022.8.16  改
	filestruct::block& blk;//feige
	int downloadingIndex=0;//下载完的个数
	asio::io_context& io_context_;
	asio::ip::tcp::socket socket_;
	string File_name;//发送的名字以及名字长度
	char recive_file_len[4096] = { 0 };//接收文件的长度	
	char* count_file_buf;//读到的字符
	string recive_file;//接收文本
	std::string fileName;   //路径+文件名
	std::size_t filelen = 0;//接收长度  类型长度，最大的长度
	size_t file_size;   //文件的大小
	int count_=0 ;   //计数
	string id_;   //id 号
	string wget_name;   //断点时的名字
	filestruct::wget_c_file wcf;

	filestruct::wget_c_file_info wcfi;  //声明一个结构体

	filestruct::wget_c_file_info wcfi1;  //声明一个结构体

	down_json dj;       //声明一个down_json类
	//QVariant file_names;
	//client_page* cli_ptr_;
};