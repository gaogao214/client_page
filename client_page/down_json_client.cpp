#include "down_json_client.h"
//#include "down_block.h"
//#include "file_server.h"
filestruct::profile downfile_path;
filestruct::files_info files_inclient;//�����ͻ��˱��ص�json�ı�


void down_json_client::receive_list(std::size_t length)
{
	OutputDebugString(L"list.json�ļ����ճɹ�");

	auto pos = list_name.find_first_of("*");
	auto name = list_name.substr(0, pos);

	//emit sign_file_name(name.data());

	auto text = list_name.substr(pos + 1);

	//ֻע������һ��156
	//complete_process_.set_value(bai);

	//emit sign_pro_bar(list_name_len, list_name_len);

	parse_server_list_json(text);
	isfile_exist(text, text.size());//�ж�list.json�ļ��Ƿ����,���ھͽ���json�ı���server��json���бȽϣ������ھͱ����ļ�
	//recive_id();//����id������
}

void down_json_client::receive_id(std::size_t length)
{

	OutputDebugString(L"id.json�ļ����ճɹ�");

	auto pos = id_name_text.find_first_of("*");
	auto name = id_name_text.substr(0, pos);

	//emit sign_file_name(name.data());

	auto text = id_name_text.substr(pos + 1);

	//emit sign_pro_bar(id_name_len, id_name_len);

	save_file(name, text);//��������
	parse_block_json(text);
	down_load();//����������̳߳������������������

}

int down_json_client::read_handle(std::size_t bytes_transferred)
{
	char flag = buffer_[0];

	switch (flag)
	{
	case 0:
	{
		//�������е�buffer_
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
void down_json_client::isfile_exist(const std::string file_buf, int buf_len)//�ж�list.json�ļ��Ƿ����,���ھͽ���json�ı���server��json���бȽϣ������ھͱ����ļ�
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
		save_file(list_json, file_buf);//��������
	}
}

std::string down_json_client::open_json_file(const std::string& json_name)//��ָ�����Ƶ�json�ı�
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
void down_json_client::save_file(const std::string& name, const std::string& file_buf)//��������
{

	std::ofstream save_file_(name, std::ios::out | std::ios::binary);

	save_file_.write(file_buf.c_str(), strlen(file_buf.c_str()) - len);

	save_file_.close();
	std::cout << name << " �ļ�����ɹ�\n";
}

void down_json_client::down_json_run(filestruct::block Files, std::string loadip, std::string loadport, const std::string& down_id)//���������ļ��Ķ˿�
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


void down_json_client::down_load()//����������̳߳������������������
{
	
	for (auto i : files_inserver.file_list)//ÿ��id(id:1 id:2)��������
	{
		id_index[i.blockid] += 1;
	}

		
	for (auto& iter : files_inserver.file_list)
	{
		//�ڱ���list.json�ı����ҵ��ͷ������ͬ������
		auto it_client = std::find_if(files_inclient.file_list.begin(), files_inclient.file_list.end(), [&](auto file) {return file.path == iter.path; });
		if (it_client == files_inclient.file_list.end() || it_client->version < iter.version)//���û���ҵ����֣����߰汾��û�з������ĸߣ�������
		{


			blks.blocks_[iter.blockid].id = iter.blockid;//����list.json�ļ���� id �浽 blks.id(����ʱ�Ͱ�һ���ļ���id  ���ִ�����)

			blks.blocks_[iter.blockid].files.push_back(iter.path);


			index[iter.blockid] += 1;          //����һ���ļ�   �������ص�����+1


			if (id_index[iter.blockid] == index[iter.blockid])//��id:1  id:2��ÿ��id�������� == �浽������     ���������ʱ��ʼ�������أ�
			{
				auto it = blks_.blocks.find(blks.blocks_[iter.blockid].id);//�ҵ�Ҫ�����ļ���id 
				//auto it = std::find_if(blks.blocks_.begin(), blks.blocks_.end(), [&](auto file) {return file == blks.blocks_[iter.blockid].id; });

				//auto it = blks_.blocks.find(blks.blocks_.begin(), blks.blocks_.end(), blks.blocks_[iter.blockid].id);//�ҵ�Ҫ�����ļ���id 
				if (it == blks_.blocks.end())//û���ҵ�
					continue;
				OutputDebugString(L"ת�ļ�io_context");

				pool.enqueue(bind(&down_json_client::down_json_run, this, blks.blocks_[iter.blockid], it->second.server.back().ip, it->second.server.back().port, std::to_string(iter.blockid)));


			}
		}
	}
}

void down_json_client::send_id_port(const std::string id_port)//���ͳ�Ϊ��������id ip port 
{
	std::size_t id_port_len = id_port.size();//id ip port�ַ�����С                       
	id_port_buf.resize(sizeof(size_t) + id_port_len);//��id_port_buf����sizeof(size_t) + id_port_len�ĳ���
	std::memcpy(id_port_buf.data(), &id_port_len, sizeof(size_t));
	sprintf(&id_port_buf[sizeof(size_t)], "%s", id_port.c_str());//���ļ�������&Id_IP_Port_buf[10]
	//socket_.async_write_some(asio::buffer(id_port_buf),
	//asio::async_write(socket_, asio::buffer(id_port_buf.data(), id_port_buf.size()),	//һ�δ����ļ������Ⱥ��ļ���
	//	[this, id_port_len, id_port](std::error_code ec, std::size_t)
	//	{
	//		cout << ec << endl;
	//		if (!ec)
	//		{
	//			std::cout << "���͸�������  id ip port:  " << id_port_buf << std::endl;
	//		}
	//	});

	this->async_write(id_port_buf, [this, id_port_len, id_port](std::error_code ec, std::size_t)
		{
			std::cout << ec << std::endl;
			if (!ec)
			{
				std::cout << "���͸�������  id ip port:  " << id_port_buf << std::endl;
			}
		});
}
