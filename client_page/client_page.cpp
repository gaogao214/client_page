#include "client_page.h"
#include "asio.hpp"
#include "down_json.h"

client_page::client_page(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	connect(ui.connect, &QPushButton::clicked, this, &client_page::request_connect);
	//connect(ui.connect,SIGNAL(clicked(bool)), this,SLOT(request_connect(bool)));
	//ui.connect->setCheckable(true);
	connect(ui.get_list, &QPushButton::clicked, this, &client_page::get_list_from_server);

	//清理本地文件
}

client_page::~client_page()
{}

void client_page::request_connect()
{
	ui.get_list->setEnabled(true);

	asio::ip::tcp::resolver resolver(io_context);

	auto endpoints = resolver.resolve("127.0.0.1", "12312");

	p_ = std::make_shared<down_json>(io_context, endpoints, this);

	p_->run();

	//ui.connect->setText(u8"已连接");

	//main_thread_ptr_->join();

}

void client_page::get_list_from_server()
{

	//向服务器发送请求，我要获取列表
	//std::cout << "d" << std::endl;
	//std::string str = "d";
	//OutputDebugStringA(str.data());
	


	//p_->get_list_from_server_();


	//ui.text_log->insertPlainText(u8"获取成功\n");
	
	//p_->get_list_from_server([this]() 
	//	{
	//		p_->down_json_profile();
	//	});

}