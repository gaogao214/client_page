#include "client_page.h"
#include <QprogressBar>
#include <QMetaObject>
#include <Qstring>
#include <QMetaType>
#include <QVariant>
#include <QListWidgetItem>

client_page::client_page(QWidget* parent)
	: QMainWindow(parent)
	, io_pool_(10)
{
	ui.setupUi(this);

	ui.pro_bar->setOrientation(Qt::Horizontal);  
	ui.pro_bar->setMinimum(0); 
	ui.pro_bar->setMaximum(100);
	qRegisterMetaType<QVariant>("QVariant");

	connect(ui.init, &QPushButton::clicked, this, &client_page::init_listview);
	//connect(ui.confirm,SIGNAL(clicked(bool)),this,SLOT(confirm_listview(bool)));
	connect(ui.connect, &QPushButton::clicked, this, &client_page::request_connect);
	
	
	start_io_pool();
}

client_page::~client_page()
{
	stop();
}

void client_page::request_connect()
{
	asio::ip::tcp::resolver resolver(io_pool_.get_io_context());
	auto endpoints = resolver.resolve("127.0.0.1", "12312");


	down_json_ptr_ = new down_json_client(io_pool_.get_io_context(), endpoints);
	
	//qRegisterMetaType<filestruct::block>("filestruct::block");
	//qRegisterMetaType<std::string>("std::string");
	qRegisterMetaType<QTextCursor>("QTextCursor");
	QMetaObject::Connection connecthanndle_pro_bar = connect(down_json_ptr_, SIGNAL(sign_pro_bar(int,int)), this, SLOT(show_progress_bar(int,int)), Qt::QueuedConnection);
	if (connecthanndle_pro_bar)
	{
		OutputDebugString(L"pro bar 信号与槽函数关联成功\n");
		ui.text_log->insertPlainText(u8"pro_bar 槽函数关联成功\n");
	}
	else
	{
		OutputDebugString(L"pro bar 关联失败\n");
	}

	QMetaObject::Connection connecthanndle_name = connect(down_json_ptr_, SIGNAL(sign_file_name(QString)), this, SLOT(show_file_name(QString)), Qt::QueuedConnection);
	if (connecthanndle_name)
	{
		OutputDebugString(L"name 信号与槽函数关联成功\n");
		ui.text_log->insertPlainText(u8"name 槽函数关联成功\n");

	}
	else
	{
		OutputDebugString(L"name 关联失败\n");
	}
	/*QMetaObject::Connection connecthanndle_log = connect(down_json_ptr_, SIGNAL(sign_text_log(QString)), this, SLOT(show_text_log(QString)), Qt::QueuedConnection);
	if (connecthanndle_log)
	{
		OutputDebugString(L"text 信号与槽函数关联成功\n");
		ui.text_log->insertPlainText(u8"text_log 槽函数关联成功\n");

	}
	else
	{
		OutputDebugString(L"text_log 关联失败\n");
	}*/



	auto connect_block_ = QObject::connect(down_json_ptr_, &down_json_client::sign_down_block, [this](QVariant var, int id, QString ip, QString port)
		{
			this->down_block_file_(var, id, ip, port);
			ui.text_log->insertPlainText(u8"block 关联成功\n");
		});


}


void client_page::down_block_file_(QVariant file_names, int id, QString loadip, QString loadport)
{
	asio::ip::tcp::resolver resolver(io_pool_.get_io_context());

	auto endpoints = resolver.resolve("127.0.0.1", "12314");
	bck = file_names.value<filestruct::block>();

	auto down_block_ptr_ = std::make_shared<down_block_client>(io_pool_.get_io_context(), endpoints, bck);
	


	qRegisterMetaType<std::size_t>("std:::size_t");

	QMetaObject::Connection connecthanndle_ = connect(down_block_ptr_.get(), SIGNAL(signal_get_id_port_externl(std::size_t, QString)), SLOT(send_get_id_port_for_server(std::size_t, QString)), Qt::DirectConnection);
	if (connecthanndle_)
	{
		ui.text_log->insertPlainText(u8"客户端转服务器 信号与槽 关联成功\n");
	}
	QMetaObject::Connection connecthanndle_wget = connect(down_block_ptr_.get(), SIGNAL(signal_wget_down_file(QString)), SLOT(wget_c_file_(QString)), Qt::DirectConnection);
	if (connecthanndle_wget)
	{
		ui.text_log->insertPlainText(u8"客户端转服务器 信号与槽 关联成功\n");
	}


	QMetaObject::Connection connecthanndle_pro_bar = connect(down_block_ptr_.get(), SIGNAL(signal_pro_bar(int,int)), this, SLOT(show_progress_bar(int,int)), Qt::QueuedConnection);
	if (connecthanndle_pro_bar)
	{
		ui.text_log->insertPlainText(u8"block _pro_bar 信号与槽 关联成功\n");
	}
	QMetaObject::Connection connecthanndle_name = connect(down_block_ptr_.get(), SIGNAL(signal_file_name_(QString)), this, SLOT(show_file_name(QString)), Qt::QueuedConnection);
	if (connecthanndle_name)
	{
		ui.text_log->insertPlainText(u8"block _file_name 信号与槽 关联成功\n");
	}



	down_block_ptr_->number_ = id;
down_block_ptr_->send_filename();

	down_blocks_.push_back(down_block_ptr_);


}


void client_page::wget_c_file_(QString wget_file_name)
{

	asio::ip::tcp::resolver resolver_(io_pool_.get_io_context());
	auto endpoint_ = resolver_.resolve( "127.0.0.1","12313" );

	wget_c_file_client_ptr_ = new wget_c_file_client(io_pool_.get_io_context(), endpoint_);
	while (!wget_c_file_client_ptr_->connect_flag_);

	std::string file_name = wget_file_name.toStdString();

	wget_c_file_client_ptr_->do_send_wget_file_name_text(file_name);

	//m_wget_c_file_ = std::make_shared<wget_c_file_client>(io_pool_.get_io_context(), endpoint_/*,this*/);
	//m_wget_c_file_->do_send_wget_file_name_text();
	//QMetaObject::Connection connecthanndle_pro_bar = connect(m_wget_c_file_.get(), SIGNAL(sign_wget_c_file_pro_bar(int, int)), this, SLOT(show_progress_bar(int, int)), Qt::QueuedConnection);
	//if (connecthanndle_pro_bar)
	//{
	//	ui.text_log->insertPlainText(u8"wget_c_file_pro_bar 信号与槽关联成功\n");
	//}
	//else
	//{
	//	OutputDebugString(L"wget_pro_bar 关联失败\n");
	//}
	//QMetaObject::Connection connecthanndle_name = connect(m_wget_c_file_.get(), SIGNAL(sign_wget_c_file_name(QString)), this, SLOT(show_file_name(QString)), Qt::QueuedConnection);
	//if (connecthanndle_name)
	//{
	//	ui.text_log->insertPlainText(u8"wget_name 槽函数关联成功\n");
	//}
	//else
	//{
	//	OutputDebugString(L"wget_name 关联失败\n");
	//}
	//QMetaObject::Connection connecthanndle_text_log = connect(m_wget_c_file_.get(), SIGNAL(sign_wget_c_file_text_log(QString)), this, SLOT(show_text_log(QString)), Qt::QueuedConnection);
	//if (connecthanndle_text_log)
	//{
	//	ui.text_log->insertPlainText(u8"wget_text_log 槽函数关联成功\n");
	//}
	//else
	//{
	//	OutputDebugString(L"wget_text_log 关联失败\n");
	//}

}

void client_page::show_progress_bar(int maxvalue_, int value_)
{

	//dpro = (ui.pro_bar->value() - ui.pro_bar->minimum()) * 100 / (ui.pro_bar->maximum() - ui.pro_bar->minimum());
	dpro = (value_ * 100.00) / maxvalue_;
	std::string str1 = std::to_string(value_);
	std::string str2 = std::to_string(maxvalue_);

	ui.pro_bar->setValue(dpro);
	ui.pro_bar->update();
	ui.pro_bar->setFormat(QString::fromLocal8Bit("当前进度为:%1%").arg(QString::number(dpro, 'f', 1)));
	ui.pro_bar->setAlignment(Qt::AlignRight | Qt::AlignCenter);


}

void client_page::show_file_name(QString file_name)
{
	ui.file_name->setText(file_name);
}

void client_page::show_text_log(QString log_)
{
	ui.text_log->insertPlainText(log_);

}

void client_page::show_connect()
{
	ui.text_log->insertPlainText(u8"连接成功");
}


void client_page::send_get_id_port_for_server(std::size_t get_server_id, QString get_server_port)
{

	OutputDebugString(L"客户端转服务器    进入槽________________________________________________\n");
	std::string str = get_server_port.toStdString();
	std::string id_port = std::to_string(get_server_id)+','+str;
	down_json_ptr_->send_id_port(/*id_port*/get_server_id,str);
}

void client_page::init_listview()
{
	std::string str;
	std::string file_path;
	QString str_;
	QListWidgetItem* item;
	parse_down_jsonfile("down.json");
	for (auto& iter : bck.files)
	{
		
		file_path = downfile_path.path + "\\" + iter;
		
		auto pos = file_path.find_last_of("\\");

		str = file_path.substr(0,pos);

		str_ = QString::fromStdString(str);
		
		item = new QListWidgetItem();
		item->setText(str_);
		item->setCheckState(Qt::Unchecked);
		ui.listwidget->addItem(item);
		
			
	}
	
}

void client_page::choose_down_path()
{




}