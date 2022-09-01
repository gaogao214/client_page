#pragma once
#include "common.h"
#include "rapidjson/filereadstream.h"
#include "GxJsonUtility.h"
#include <QVariant>
#include <fstream>

namespace filestruct {
	struct list_json {                
		std::string path;
		uint32_t version;
		uint32_t blockid;
		GX_JSON(path, version, blockid);
	};

	struct files_info                  
	{
		std::vector<list_json> file_list;
		GX_JSON(file_list);
	};

	struct id_json {                  
		std::string ip;
		std::string port;
		GX_JSON(ip, port)
	};
	struct block_info {               
		int id;
		std::vector<id_json> server;
		GX_JSON(id, server)
	};
	struct blocks {                    
		std::unordered_map<int, block_info> blocks;
		//vector<block_info> blocks;
		GX_JSON(blocks);
	};
	struct profile {                 
		std::string path;
		std::string port;
		GX_JSON(path, port);
	};

	struct profile_info             
	{
		std::vector<profile> file_list;
		GX_JSON(file_list);
	};

	struct block                     
	{
		int id;
		std::vector<std::string> files;
	};

	struct blocks_for_download
	{
		std::unordered_map<int, block> blocks_;
	};
	struct wget_c_file                
	{
		std::string wget_name;
		uint32_t offset;	
		
		GX_JSON(wget_name, offset);
	};
	struct wget_c_file_info
	{
		std::vector<wget_c_file> wget_c_file_list;
		GX_JSON(wget_c_file_list);
	};
}
inline  filestruct::profile downfile_path;      
inline filestruct::files_info files_inclient;
inline filestruct::files_info files_inserver;		
inline filestruct::blocks blks_;	
inline filestruct::wget_c_file_info wcfi;  

inline std::string open_json_file(const std::string& json_name)
{
	std::string content{};
	std::string tmp{};
	std::fill(content.begin(), content.end(), 0);

	std::fstream ifs(json_name, std::ios::in | std::ios::binary);
	if (!ifs.is_open())
		return {};

	while (std::getline(ifs, tmp))
	{
		content.append(tmp);
	}

	return content;

}
inline void parse_client_list_json(std::string name)
{
	std::string readbuffer = open_json_file(name);
	files_inclient.deserializeFromJSON(readbuffer.c_str());
}

inline void parse_server_list_json(const char* text_json)
{
	files_inserver.deserializeFromJSON(text_json);
}

inline void parse_down_jsonfile(std::string name)
{
	std::string readbuffer = open_json_file(name);
	downfile_path.deserializeFromJSON(readbuffer.c_str());
}

inline void parse_block_json(const char* text_json)
{
	blks_.deserializeFromJSON(text_json);
}

inline filestruct::wget_c_file_info  parse_wget_c_file_json(const std::string& name)//打开断点续传文件  解析json文件
{
	std::string readbuffer = open_json_file(name);

	wcfi.deserializeFromJSON(readbuffer.c_str());

	return wcfi;
}

inline void save_file(const char* name, const char* file_buf)//保存内容
{
	volatile int len = 0;

	std::ofstream save_file_(name, std::ios::out | std::ios::binary);

	save_file_.write(file_buf, strlen(file_buf) - len);

	save_file_.close();

}

inline std::size_t send_file_len(const std::string& filename)
{
	std::ifstream infile(filename.c_str());
	infile.seekg(0, std::ios_base::end);
	size_t fsize = infile.tellg();//list.json文本的大小
	infile.close();

	return fsize;
}

inline std::string send_file_context(const std::string& filename)//文本的内容
{

	std::ifstream File(filename.c_str());
	char file_buf = '0';//list.json文件
	std::string buf;//一个一个读之后存在这里，list.json文本

	while (File.get(file_buf))
	{
		buf.push_back(file_buf);
	}
	File.close();

	return buf;
}

inline void json_formatting(std::string& strtxt)//按照格式写入 json 文件
{
	unsigned int dzkh = 0; //括号的计数器
	bool isy = false; //是不是引号
	for (int i = 0; i < strtxt.length(); ++i) {
		if (isy || strtxt[i] == '"') // "前引号 "后引号
		{
			if (strtxt[i] == '"')
				isy = !isy;
			continue;
		}
		std::string tn = "";

#define ADD_CHANGE                          \
    for (unsigned int j = 0; j < dzkh; ++j) \
        tn += "\t";

		if (strtxt[i] == '{' || strtxt[i] == '[') {
			dzkh++;
			ADD_CHANGE
				strtxt = strtxt.substr(0, i + 1) + "\n" + tn + strtxt.substr(i + 1);
			i += dzkh + 1;
		}
		else if (strtxt[i] == '}' || strtxt[i] == ']') {
			dzkh--;
			ADD_CHANGE
				strtxt = strtxt.substr(0, i) + "\n" + tn + strtxt.substr(i);
			i += dzkh + 1;
		}
		else if (strtxt[i] == ',') {
			ADD_CHANGE
				strtxt = strtxt.substr(0, i + 1) + "\n" + tn + strtxt.substr(i + 1);
			i += dzkh + 1;
		}
	}
}
