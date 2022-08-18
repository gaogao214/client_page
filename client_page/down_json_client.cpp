#include "down_json_client.h"
//#include "down_block.h"
//#include "file_server.h"
filestruct::profile downfile_path;
filestruct::files_info files_inclient;//解析客户端本地的json文本


void down_json_client::receive_list(std::size_t length)
{
	OutputDebugString(L"list.json文件接收成功");

	auto pos = list_name.find_first_of("*");
	auto name = list_name.substr(0, pos);

	//emit sign_file_name(name.data());

	auto text = list_name.substr(pos + 1);

	//只注释了这一行156
	//complete_process_.set_value(bai);

	//emit sign_pro_bar(list_name_len, list_name_len);

	parse_server_list_json(text);
	isfile_exist(text, text.size());//判断list.json文件是否存在,存在就解析json文本与server的json进行比较，不存在就保存文件
	//recive_id();//接收id的名字
}

void down_json_client::receive_id(std::size_t length)
{

	OutputDebugString(L"id.json文件接收成功");

	auto pos = id_name_text.find_first_of("*");
	auto name = id_name_text.substr(0, pos);

	//emit sign_file_name(name.data());

	auto text = id_name_text.substr(pos + 1);

	//emit sign_pro_bar(id_name_len, id_name_len);

	save_file(name, text);//保存内容
	parse_block_json(text);
	down_load();//把任务放在线程池里向服务器请求下载

}

int down_json_client::read_handle(std::size_t bytes_transferred)
{
	char flag = buffer_[0];

	switch (flag)
	{
	case 0:
	{
		//处理所有的buffer_
		receive_list(bytes_transferred);
	}
	break;
	case 1:
		receive_id(bytes_transferred);

		break;
	default:
		break;
	}
	return 0;
}
void down_json_client::isfile_exist(const std::string file_buf, int buf_len)//判断list.json文件是否存在,存在就解析json文本与server的json进行比较，不存在就保存文件
{

	std::fstream list("list.json", std::ios::binary | std::ios::out | std::ios::app);
	if (!list.is_open())
	{
		while (file_buf.c_str() != nullptr)
		{
			list.write(file_buf.c_str(), buf_len);
		}
		list.flush();
		list.close();
	}
	else {
		parse_client_list_json(list_json);
		save_file(list_json, file_buf);//保存内容
	}
}

std::string down_json_client::open_json_file(const std::string& json_name)//打开指定名称的json文本
{
	std::string content{};
	std::string tmp{};
	std::fill(content.begin(), content.end(), 0);//清空

	std::fstream ifs(json_name, std::ios::in | std::ios::binary);
	if (!ifs.is_open())
		return {};

	while (std::getline(ifs, tmp))
	{
		content.append(tmp);
	}

	return content;

}
void down_json_client::save_file(const std::string& name, const std::string& file_buf)//保存内容
{

	std::ofstream save_file_(name, std::ios::out | std::ios::binary);

	save_file_.write(file_buf.c_str(), strlen(file_buf.c_str()) - len);

	save_file_.close();
	std::cout << name << " 文件保存成功\n";
}

void down_json_client::down_json_run(filestruct::block Files, std::string loadip, std::string loadport, const std::string& down_id)//连接下载文件的端口
{
	try {
		//QVariant var;
		//var.setValue(Files);

		///*for (int i = 0; i < bck.files.size(); i++)
		//{
		//	cout << bck.files[i] << endl;
		//}*/
		//QString ip = QString::fromStdString(loadip);
		//QString port = QString::fromStdString(loadport);
		//emit sign_down_block(var, ip, port);
		/*asio::io_context ios;
		asio::ip::tcp::resolver resolver_(ios);
		auto endpoint = resolver_.resolve({ loadip,loadport });
		down_block db(ios, endpoint, Files);

		ios.run();*/
	}
	catch (...)
	{
		parse_down_jsonfile(down_json_name);
		/*while (!*/send_id_port(down_id + "," + downfile_path.port);//)
		//	continue;
	}
}


void down_json_client::down_load()//把任务放在线程池里向服务器请求下载
{
	
	for (auto i : files_inserver.file_list)//每个id(id:1 id:2)的总数量
	{
		id_index[i.blockid] += 1;
	}

		
	for (auto& iter : files_inserver.file_list)
	{
		//在本地list.json文本里找到和服务端相同的名字
		auto it_client = std::find_if(files_inclient.file_list.begin(), files_inclient.file_list.end(), [&](auto file) {return file.path == iter.path; });
		if (it_client == files_inclient.file_list.end() || it_client->version < iter.version)//如果没有找到名字，或者版本号没有服务器的高，就下载
		{


			blks.blocks_[iter.blockid].id = iter.blockid;//遍历list.json文件里的 id 存到 blks.id(下载时就把一个文件的id  名字存起来)

			blks.blocks_[iter.blockid].files.push_back(iter.path);


			index[iter.blockid] += 1;          //下载一个文件   就让下载的数量+1


			if (id_index[iter.blockid] == index[iter.blockid])//（id:1  id:2）每个id的总数量 == 存到的数量     （数量相等时开始请求下载）
			{
				auto it = blks_.blocks.find(blks.blocks_[iter.blockid].id);//找到要下载文件的id 
				//auto it = std::find_if(blks.blocks_.begin(), blks.blocks_.end(), [&](auto file) {return file == blks.blocks_[iter.blockid].id; });

				//auto it = blks_.blocks.find(blks.blocks_.begin(), blks.blocks_.end(), blks.blocks_[iter.blockid].id);//找到要下载文件的id 
				if (it == blks_.blocks.end())//没有找到
					continue;
				OutputDebugString(L"转文件io_context");

				pool.enqueue(bind(&down_json_client::down_json_run, this, blks.blocks_[iter.blockid], it->second.server.back().ip, it->second.server.back().port, std::to_string(iter.blockid)));


			}
		}
	}
}

void down_json_client::send_id_port(const std::string id_port)//发送成为服务器的id ip port 
{
	std::size_t id_port_len = id_port.size();//id ip port字符串大小                       
	id_port_buf.resize(sizeof(size_t) + id_port_len);//给id_port_buf分配sizeof(size_t) + id_port_len的长度
	std::memcpy(id_port_buf.data(), &id_port_len, sizeof(size_t));
	sprintf(&id_port_buf[sizeof(size_t)], "%s", id_port.c_str());//把文件名赋给&Id_IP_Port_buf[10]
	//socket_.async_write_some(asio::buffer(id_port_buf),
	//asio::async_write(socket_, asio::buffer(id_port_buf.data(), id_port_buf.size()),	//一次传输文件名长度和文件名
	//	[this, id_port_len, id_port](std::error_code ec, std::size_t)
	//	{
	//		cout << ec << endl;
	//		if (!ec)
	//		{
	//			std::cout << "发送给服务器  id ip port:  " << id_port_buf << std::endl;
	//		}
	//	});

	this->async_write(id_port_buf, [this, id_port_len, id_port](std::error_code ec, std::size_t)
		{
			std::cout << ec << std::endl;
			if (!ec)
			{
				std::cout << "发送给服务器  id ip port:  " << id_port_buf << std::endl;
			}
		});
}
