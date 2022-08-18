#include "down_block_client.h"
#include "file_server.h"
void down_block_client::send_filename()
{
	if (downloadingIndex > blk.files.size())
		return;

	if (downloadingIndex == blk.files.size())
	{
		std::string id_ = std::to_string(blk.id);
		client_to_server(downfile_path.port);
		//dj.send_id_port(id_+","+downfile_path.port);
		return;
	}

	std::string name = blk.files.at(downloadingIndex++);
	//emit signal_file_name_(QString::fromStdString(name));
	size_t name_len = name.size();
	file_name.resize(sizeof(size_t) + name_len);
	std::memcpy(file_name.data(), &name_len, sizeof(size_t));
	sprintf(&file_name[sizeof(size_t)], "%s", name.data());

	this->async_write(file_name, [this,name](std::error_code ec, std::size_t)
	{
			if (!ec)
			{
				std::cout << "\n客户端请求下载  " << name << "文件\n";
				does_the_folder_exist(name);
			}

	});

}

void down_block_client::recive_file_text(const std::string& fname, int recive_len, const std::string& no_path_added_name)
{


}

int down_block_client::read_handle(std::size_t bytes_transferred)
{

	return 0;
}

void down_block_client::does_the_folder_exist(const std::string& list_name)//判断文件夹是否存在，不存在则创建文件夹
{
	file_path = downfile_path.path + "\\" + list_name;
	std::size_t found = file_path.find_last_of("\\");

	std::error_code ec;
	if (!std::filesystem::exists(file_path.substr(0, found)))//不存在创建
	{
		std::filesystem::path{ file_path.substr(0, found) }.parent_path();
		// std::boolalpha; std::filesystem::exists(path);
		std::filesystem::create_directories(file_path.substr(0, found), ec);
		std::cout << "文件夹创建成功\n";
	}
	recive_file_text(file_path, 4096, list_name);
}

//断开再连接时     wcfi 清空  断开连接时，保存到一个文件中 ，连接时，先读这个文件   再把这个保存到别的文件中
//保存到wget_c_file文件中 下载完成的文件名  和偏移量
void down_block_client::save_location(const string& name, const string& no_path_add_name)
{


	ifstream id_File(name, ios::binary);
	id_File.seekg(0, ios_base::end);
	size_t file_size = id_File.tellg();//文本的大小
	id_File.seekg(0, ios_base::beg);
	//cout << "\nfile_size_===" << file_size << endl;


	wcf.wget_name = no_path_add_name;
	wcf.offset = file_size;

	wcfi1.wget_c_file_list.push_back(wcf);
	save_wget_c_file_json(wcfi1, "wget_c_file1.json");

}

/*记录暂停下载时的  文件名以及偏移量  */
void down_block_client::Breakpoint_location()
{
	/*把断点后的文件名也保存在wget_c_file中*/
	for (auto& iter : files_inclient.file_list)
	{

		//在本地list.json文本里找到和服务端相同的名字
		auto it_client = std::find_if(wcfi1.wget_c_file_list.begin(), wcfi1.wget_c_file_list.end(), [&](auto file) {return file.wget_name == iter.path; });
		if (it_client == wcfi1.wget_c_file_list.end())//如果没有找到名字，就把文件名添加到wget_c_file这个文件中
		{
			if (iter.path.empty())
				continue;

			wcf.wget_name = iter.path;
			wcf.offset = 0;
			wcfi1.wget_c_file_list.push_back(wcf);
		}
	}
	save_wget_c_file_json(wcfi1, "wget_c_file.json");

}

void down_block_client::save_wget_c_file_json(filestruct::wget_c_file_info wcfi, const string& name)
{
	string text = RapidjsonToString(wcfi.serializeToJSON());
	gsh(text);

	auto file = fopen(name.c_str(), "wb");

	const char* t = text.c_str();
	size_t length = text.length();
	fwrite(t, length, 1, file);
	fflush(file);
	fclose(file);


}

void down_block_client::client_to_server(string profile_port)//开一个线程，客户端转换成服务端
{
	std::thread t(std::bind(&down_block_client::server, this, profile_port));
	/*	t.join();*/
	//std::cout << "客户端: id: " << id_ << "  端口号: " << profile_port << " 变成服务端\n";
	t.detach();
}

void down_block_client::server(const std::string& server_port)//客户端转换成服务端
{
	int port = atoi(server_port.c_str());
	asio::io_context io_context;//创建对象，用来连接上下文
	asio::ip::tcp::endpoint _endpoint(asio::ip::tcp::v4(), port);
	auto fs = std::make_shared<file_server>(io_context, _endpoint);

	io_context.run();
}


void down_block_client::gsh(std::string& strtxt)//按照格式写入 json 文件
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

