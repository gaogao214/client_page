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

	void does_the_folder_exist(const std::string& list_name);//�ж��ļ����Ƿ���ڣ��������򴴽��ļ���

	void client_to_server(std::string profile_port);    //��һ���̣߳��ͻ���ת���ɷ����

	void server(const std::string& server_port);      //�ͻ���ת���ɷ����

	void save_location(const std::string& name, const std::string& no_path_add_name);    /*��¼��ͣ����ʱ��  �ļ����Լ�ƫ����  */

	void save_wget_c_file_json(filestruct::wget_c_file_info wcfi, const std::string& name);

	void Breakpoint_location();    /*��¼��ͣ����ʱ��  �ļ����Լ�ƫ����  */

	void gsh(std::string& strtxt);//���ո�ʽд��id.json �ļ�


private:
	filestruct::block& blk;//feige
	filestruct::wget_c_file wcf;

	filestruct::wget_c_file_info wcfi;  //����һ���ṹ��

	filestruct::wget_c_file_info wcfi1;  //����һ���ṹ��

	down_json_client dj;       //����һ��down_json��

	int downloadingIndex = 0;//������ĸ���
	std::string file_name;
	std::string file_path;   //·��+�ļ���


};

