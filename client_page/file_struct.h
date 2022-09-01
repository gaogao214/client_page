#pragma once
#include "common.h"
#include "rapidjson/filereadstream.h"
#include "GxJsonUtility.h"
#include <QVariant>
#include <fstream>

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
	/*	bool operator==(const wget_c_file& a) {
			wget_name.pop_back();
			return this->wget_name == a.wget_name;
		}*/
		uint32_t offset;	
		
		
		//bool operator<(const wget_c_file& b)const { return  offset < b.offset; }
		GX_JSON(wget_name, offset);
	};
	struct wget_c_file_info
	{
		std::vector<wget_c_file> wget_c_file_list;
		//std::set<wget_c_file> wget_c_file_list;
		GX_JSON(wget_c_file_list);
	};
}

//filestruct::files_info files_inserver;		//������������json�ı�
//filestruct::profile downfile_path;      //�����ļ�
//
//filestruct::blocks blks_;					//����id.json�ı� 
inline filestruct::files_info files_inclient;

inline std::string open_json_file(const std::string& json_name)//��ָ�����Ƶ�json�ı�
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
inline void parse_client_list_json(std::string name)//��list_json   json�ļ�  ����json�ļ�
{
	std::string readbuffer = open_json_file(name);
	files_inclient.deserializeFromJSON(readbuffer.c_str());
}
//void  parse_server_list_json(const char* text_json)//��list_json   json�ļ�  ����json�ļ�
//{
//	files_inserver.deserializeFromJSON(text_json);
//}
//
//void  parse_down_jsonfile(std::string name)//�������ļ������ҵ������ļ��е�·��,�鿴·���µ��ļ����ļ���   ����json�ļ�
//{
//	std::string readbuffer = open_json_file("down.json");
//	downfile_path.deserializeFromJSON(readbuffer.c_str());
//}
//
//void parse_block_json(const char* text_json)//��list_json   json�ļ�  ����json�ļ�
//{
//	blks_.deserializeFromJSON(text_json);
//}