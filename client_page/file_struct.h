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
	struct profile {                  //配置文件
		std::string path;
		std::string port;
		GX_JSON(path, port);
	};

	struct profile_info               //配置文件
	{
		std::vector<profile> file_list;
		GX_JSON(file_list);
	};

	struct block                      //存一个块id的文件名
	{
		int id;
		std::vector<std::string> files;
	};

	struct blocks_for_download
	{
		std::unordered_map<int, block> blocks_;
	};
	struct wget_c_file                 //断点续传文件
	{
		std::string wget_name;
		uint32_t offset;
		bool operator<(const wget_c_file& b)const { return  offset < b.offset; }
		GX_JSON(wget_name, offset);
	};
	struct wget_c_file_info
	{
		//std::vector<wget_c_file> wget_c_file_list;
		std::set<wget_c_file> wget_c_file_list;
		GX_JSON(wget_c_file_list);
	};
}
