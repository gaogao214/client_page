#include "down_json_client.h"
#include "down_block_client.h"
//#include "file_server.h"

filestruct::profile downfile_path;
filestruct::files_info files_inclient;//�����ͻ��˱��ص�json�ı�

void down_json_client::receive_buffer(std::size_t length)
{
	std::string str(buffer_.data(),length);
	std::string buf = str.substr(1);
	auto pos = buf.find_first_of("*");
	auto name = buf.substr(0, pos);

	emit sign_file_name(name.data());

	auto text = buf.substr(pos + 1);

	emit sign_pro_bar(length, length);
	char flag = buffer_[0];
	switch (flag)
	{
	case '0':
	{
		//�������е�buffer_
		parse_server_list_json(text);
		isfile_exist(text, text.size());//�ж�list.json�ļ��Ƿ����,���ھͽ���json�ı���server��json���бȽϣ������ھͱ����ļ�

	}
	break;
	case '1':
		save_file(name, text);//��������
		parse_block_json(text);
		down_load();//����������̳߳������������������

		break;
	default:
		break;
	}

	
	
}

int down_json_client::read_handle(std::size_t bytes_transferred)
{

	receive_buffer(bytes_transferred);


	//if (func_)
	//	func_();

	return 0;
}


void  down_json_client::parse_server_list_json(std::string text_json)//��list_json   json�ļ�  ����json�ļ�
{
	files_inserver.deserializeFromJSON(text_json.c_str());
}

void  down_json_client::parse_down_jsonfile(std::string name)//�������ļ������ҵ������ļ��е�·��,�鿴·���µ��ļ����ļ���   ����json�ļ�
{
	std::string readbuffer = open_json_file("down.json");
	downfile_path.deserializeFromJSON(readbuffer.c_str());
}

void  down_json_client::parse_block_json(std::string text_json)//��list_json   json�ļ�  ����json�ļ�
{
	blks_.deserializeFromJSON(text_json.c_str());
}

void down_json_client::parse_client_list_json(std::string name)//��list_json   json�ļ�  ����json�ļ�
{
	std::string readbuffer = open_json_file(name);
	files_inclient.deserializeFromJSON(readbuffer.c_str());
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
		//
		//QString ip = QString::fromStdString(loadip);
		//QString port = QString::fromStdString(loadport);

		//emit sign_down_block(var,ip, port);

		asio::ip::tcp::resolver resolver(get_io_context());

		auto endpoints = resolver.resolve(loadip, loadport);

		///*bck = file_names.value<filestruct::block>();
		//*/
		auto down_block_ptr_ = std::make_shared<down_block_client>(get_io_context(), endpoints, Files);

		down_block_list_.push_back(down_block_ptr_);

		
		down_block_ptr_->send_filename();
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

		
	//auto iter = *files_inserver.file_list.begin();
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

				//down_json_run(blks.blocks_[iter.blockid], it->second.server.back().ip, it->second.server.back().port, std::to_string(iter.blockid));
			}
		}
	}

	/*QString ip = QString::fromStdString("");
	QString port = QString::fromStdString("");

	emit sign_down_block(ip, port);*/
}

void down_json_client::send_id_port(const std::string id_port)//���ͳ�Ϊ��������id ip port 
{
	std::size_t id_port_len = id_port.size();//id ip port�ַ�����С                       
	id_port_buf.resize(sizeof(size_t) + id_port_len);//��id_port_buf����sizeof(size_t) + id_port_len�ĳ���
	std::memcpy(id_port_buf.data(), &id_port_len, sizeof(size_t));
	sprintf(&id_port_buf[sizeof(size_t)], "%s", id_port.c_str());//���ļ�������&Id_IP_Port_buf[10]

	this->async_write(id_port_buf, [this, id_port_len, id_port](std::error_code ec, std::size_t)
		{
			std::cout << ec << std::endl;
			if (!ec)
			{
				std::cout << "���͸�������  id ip port:  " << id_port_buf << std::endl;
			}
		});
}
