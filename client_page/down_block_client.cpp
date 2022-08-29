#include "down_block_client.h"
#include "file_server.h"
#include "request.hpp"



filestruct::wget_c_file_info down_block_client::wcfi_copy ;
filestruct::wget_c_file down_block_client::wcf;
std::mutex down_block_client::write_mtx_;
std::unordered_map<std::size_t, std::vector<std::string>> down_block_client::id_to_the_files;
std::unordered_map<std::size_t, std::vector<std::string>> down_block_client::total_id_files_num;
filestruct::block down_block_client::blk_copy;
down_block_client* down_block_client::client_=nullptr;



void down_block_client::send_filename()
{
	blk_copy = blk;
	

	for (auto iter : blk.files)
	{	
		auto name = iter;

		total_id_files_num[blk.id].push_back(name);

		if (name.empty())
			continue;
		
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(200ms);


		std::string str = std::to_string(blk.id) + name ;
		std::size_t str_len = str.size();


		//request req;
		//req.header_.id_ = blk.id;
		//memcpy(req.header_.name_,&name,name.size());
		

		file_name.resize(sizeof(size_t) + str_len);
		//std::memcpy(file_name.data(), &str_len, sizeof(size_t));
		sprintf(&file_name[0], "%s", str.data());


//		this->async_write(req.to_bytes(req.header_), [name, this](std::error_code ec, std::size_t)

		this->async_write(file_name, [name, this](std::error_code ec, std::size_t)
			{
				if (!ec)
				{
					does_the_folder_exist(name);
					OutputDebugStringA(name.data());
					OutputDebugString(L"\n");
				
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
	/*std::string*/ id = str.substr(0, 1);
	/*std::string*/ read_name = str.substr(1,16);      //����
	/*std::string*/ file_path_ = downfile_path.path + "\\" + read_name;
	std::string total_num = str.substr(17,8);       // �����
	std::size_t total_num_{};
	std::memcpy(&total_num_, total_num.data(), sizeof(std::size_t));

	std::string text_ = str.substr(25);           //����

	/*size_t*/ id_num = atoi(id.data());

	//id_to_the_files[id_num].push_back(read_name);        //���� id ��  �� ����   �����ж�id���ļ��Ƿ�������
	
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
				//������ һ��id�ŵ��ļ�   �ͻ��˱�ɷ�����
				
				save_location(file_path_, read_name,id_num);
				//emit signal_get_server_id_port(id_num, "12314");

				//Sleep(10);
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
	save_location(file_path_, read_name,id_num);

	Breakpoint_location();

	return 0;
}


//�Ͽ�������ʱ     wcfi ���  �Ͽ�����ʱ�����浽һ���ļ��� ������ʱ���ȶ�����ļ�   �ٰ�������浽����ļ���
//���浽wget_c_file�ļ��� ������ɵ��ļ���  ��ƫ����
//��������Ҫ�������Ա�������ó�ȫ�ֺ����������������ɹ�����Ա�������ó�static  ��Ա����Ҳ��Ҫ���ó�static��
void down_block_client::save_location(const string& name, const string& no_path_add_name,std::size_t id_num)
{
	
	ifstream id_File(name, ios::binary);
	id_File.seekg(0, ios_base::end);
	size_t file_size = id_File.tellg();//�ı��Ĵ�С
	id_File.seekg(0, ios_base::beg);


	wcf.wget_name = no_path_add_name;
	wcf.offset = file_size;

	write_mtx_.lock();

	wcfi_copy.wget_c_file_list.push_back(wcf);

	write_mtx_.unlock();



	id_to_the_files[id_num].push_back(no_path_add_name);        //���� id ��  �� ����   �����ж�id���ļ��Ƿ�������

	if (total_id_files_num[id_num].size() == id_to_the_files[id_num].size())
	{
		client_to_server(downfile_path.port);

		QString get_port = QString::fromStdString(downfile_path.port);

		
		emit client_->signal_get_id_port_externl(id_num, get_port);
		
		OutputDebugString(L"id ���������  �ͻ���ת������");
	}

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

void down_block_client::save_wget_c_file_json(filestruct::wget_c_file_info wcfi,  string name)
{
	string text = RapidjsonToString(wcfi.serializeToJSON());
	json_formatting(text);


	auto file = fopen(name.c_str(), "wb");

	const char* t = text.c_str();
	size_t length = text.length();

	fwrite(t, length, 1, file);

	fflush(file);
	fclose(file);


}

void down_block_client::client_to_server(string profile_port)//��һ���̣߳��ͻ���ת���ɷ����
{
	std::thread t(std::bind(&down_block_client::server, profile_port));
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


void down_block_client::json_formatting(std::string& strtxt)//���ո�ʽд�� json �ļ�
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

