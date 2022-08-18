#pragma once
#include <asio.hpp>
#include "basic_client.h"

#include "down_json_client.h"
class down_block_client : public basic_client
{
public:

	void send_filename();

	void recive_file_text(const std::string& fname, int recive_len, const std::string& no_path_added_name);

protected:

	virtual int read_handle(std::size_t bytes_transferred) override;

private:

	void does_the_folder_exist(const std::string& list_name);//判断文件夹是否存在，不存在则创建文件夹

	void client_to_server(std::string profile_port);    //开一个线程，客户端转换成服务端

	void server(const std::string& server_port);      //客户端转换成服务端

	void save_location(const std::string& name, const std::string& no_path_add_name);    /*记录暂停下载时的  文件名以及偏移量  */

	void save_wget_c_file_json(filestruct::wget_c_file_info wcfi, const std::string& name);

	void Breakpoint_location();    /*记录暂停下载时的  文件名以及偏移量  */

	void gsh(std::string& strtxt);//按照格式写入id.json 文件


private:
	filestruct::block& blk;//feige
	filestruct::wget_c_file wcf;

	filestruct::wget_c_file_info wcfi;  //声明一个结构体

	filestruct::wget_c_file_info wcfi1;  //声明一个结构体

	down_json_client dj;       //声明一个down_json类

	int downloadingIndex = 0;//下载完的个数
	std::string file_name;
	std::string file_path;   //路径+文件名


};

