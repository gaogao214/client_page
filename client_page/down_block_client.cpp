#include "down_block_client.h"
#include "file_server.h"
void down_block_client::send_filename()
{
	//if (downloadingIndex > blk.files.size())
	//	return;

	//if (downloadingIndex == blk.files.size())
	//{
	//	//std::string id_ = std::to_string(blk.id);
	//	//client_to_server(downfile_path.port);
	//	//dj.send_id_port(id_+","+downfile_path.port);
	//	return;
	//}

	// name = blk.files.at(downloadingIndex++);

	// if (name.empty())
	//	 return;

	////emit signal_file_name_(QString::fromStdString(name));
	//size_t name_len = name.size();
	//file_name.resize(sizeof(size_t) + name_len);
	//std::memcpy(file_name.data(), &name_len, sizeof(size_t));
	//sprintf(&file_name[sizeof(size_t)], "%s", name.data());

	//this->async_write(file_name, [&, this](std::error_code ec, std::size_t)
	//{
	//		if (!ec)
	//		{
	//		
	//			does_the_folder_exist(name);
	//			
	//		}
	//});

	//send_filename();


	for (auto iter : blk.files)
	{
		auto name = iter;

		if (name.empty())
			continue;

		size_t name_len = name.size();
		file_name.resize(sizeof(size_t) + name_len);
		std::memcpy(file_name.data(), &name_len, sizeof(size_t));
		sprintf(&file_name[sizeof(size_t)], "%s", name.data());

		this->async_write(file_name, [name, this](std::error_code ec, std::size_t)
			{
				if (!ec)
				{
					does_the_folder_exist(name);
				}
			});
	}
}

void down_block_client::does_the_folder_exist(const std::string& list_name)//�ж��ļ����Ƿ���ڣ��������򴴽��ļ���
{
	file_path = downfile_path.path + "\\" + list_name;
	std::size_t found = file_path.find_last_of("\\");

	std::error_code ec;
	if (!std::filesystem::exists(file_path.substr(0, found)))//�����ڴ���
	{
		std::filesystem::path{ file_path.substr(0, found) }.parent_path();
		// std::boolalpha; std::filesystem::exists(path);
		std::filesystem::create_directories(file_path.substr(0, found), ec);
		std::cout << "�ļ��д����ɹ�\n";
	}
	//recive_file_text(file_path, 4096, list_name);
}


void down_block_client::recive_file_text(size_t recive_len)
{
	
	std::string str(buffer_.data(), recive_len);
	/*std::string*/ read_name = str.substr(0,16);      //����
	/*std::string*/ file_path_ = downfile_path.path + "\\" + read_name;
	std::string total_num = str.substr(16,8);       // �����
	std::size_t total_num_{};
	std::memcpy(&total_num_, total_num.data(), sizeof(std::size_t));

	std::string text_ = str.substr(24);           //����


	
	map_.emplace(read_name, total_num_);        
	std::ofstream file(file_path_.data(), std::ios::out | std::ios::binary | std::ios::app);


	for (auto iter = map_.begin(); iter != map_.end(); iter++)
	{
		if (iter->first == read_name )
		{

			recive_len = recive_len - 8 - 16 ;
			file.write(text_.data(), recive_len);
			++count;

			if (iter->second == count)
			{

				file.close();
				save_location(file_path_, read_name);
				count = 0;
			}
				
		}
	}

}

int down_block_client::read_handle(std::size_t bytes_transferred)
{
	recive_file_text(bytes_transferred);

	return 0;
}

int down_block_client::read_error()
{
	wcfi = parse_wget_c_file_json("wget_c_file1.json");


	for (auto& iter : wcfi.wget_c_file_list)
	{
		wcf.wget_name = iter.wget_name;
		wcf.offset = iter.offset;
		wcfi_copy.wget_c_file_list.push_back(wcf);
	}
	save_location(file_path_, read_name);

	Breakpoint_location();

	return 0;
}


//�Ͽ�������ʱ     wcfi ���  �Ͽ�����ʱ�����浽һ���ļ��� ������ʱ���ȶ�����ļ�   �ٰ�������浽����ļ���
//���浽wget_c_file�ļ��� ������ɵ��ļ���  ��ƫ����
void down_block_client::save_location(const string& name, const string& no_path_add_name)
{


	ifstream id_File(name, ios::binary);
	id_File.seekg(0, ios_base::end);
	size_t file_size = id_File.tellg();//�ı��Ĵ�С
	id_File.seekg(0, ios_base::beg);
	//cout << "\nfile_size_===" << file_size << endl;


	wcf.wget_name = no_path_add_name;
	wcf.offset = file_size;

	wcfi_copy.wget_c_file_list.push_back(wcf);
	save_wget_c_file_json(wcfi_copy, "wget_c_file1.json");

}

/*��¼��ͣ����ʱ��  �ļ����Լ�ƫ����  */
void down_block_client::Breakpoint_location()
{
	/*�Ѷϵ����ļ���Ҳ������wget_c_file��*/
	for (auto& iter : files_inclient.file_list)
	{

		//�ڱ���list.json�ı����ҵ��ͷ������ͬ������
		auto it_client = std::find_if(wcfi_copy.wget_c_file_list.begin(), wcfi_copy.wget_c_file_list.end(), [&](auto file) {return file.wget_name == iter.path; });
		if (it_client == wcfi_copy.wget_c_file_list.end())//���û���ҵ����֣��Ͱ��ļ�����ӵ�wget_c_file����ļ���
		{
			if (iter.path.empty())
				continue;

			wcf.wget_name = iter.path;
			wcf.offset = 0;
			wcfi_copy.wget_c_file_list.push_back(wcf);
		}
	}
	save_wget_c_file_json(wcfi_copy, "wget_c_file.json");

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

void down_block_client::client_to_server(string profile_port)//��һ���̣߳��ͻ���ת���ɷ����
{
	std::thread t(std::bind(&down_block_client::server, this, profile_port));
	/*	t.join();*/
	//std::cout << "�ͻ���: id: " << id_ << "  �˿ں�: " << profile_port << " ��ɷ����\n";
	t.detach();
}

void down_block_client::server(const std::string& server_port)//�ͻ���ת���ɷ����
{
	int port = atoi(server_port.c_str());
	asio::io_context io_context;//����������������������
	asio::ip::tcp::endpoint _endpoint(asio::ip::tcp::v4(), port);
	auto fs = std::make_shared<file_server>(io_context, _endpoint);

	io_context.run();
}


void down_block_client::gsh(std::string& strtxt)//���ո�ʽд�� json �ļ�
{
	unsigned int dzkh = 0; //���ŵļ�����
	bool isy = false; //�ǲ�������
	for (int i = 0; i < strtxt.length(); ++i) {
		if (isy || strtxt[i] == '"') // "ǰ���� "������
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

