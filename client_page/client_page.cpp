#include "client_page.h"
#include <QprogressBar>
#include <QMetaObject>
#include <Qstring>
#include <QMetaType>
#include <QVariant>

client_page::client_page(QWidget* parent)
	: QMainWindow(parent)
	, io_pool_(10)
{
	ui.setupUi(this);

	ui.pro_bar->setOrientation(Qt::Horizontal);  //进度条水平方向
	ui.pro_bar->setMinimum(0);  //最小值
	qRegisterMetaType<QVariant>("QVariant");
	connect(ui.connect, &QPushButton::clicked, this, &client_page::request_connect);
	connect(ui.go_on, &QPushButton::clicked, this, &client_page::wget_c_file_);
	//清理本地文件

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


	down_json_ptr_ = std::make_shared<down_json_client>(io_pool_.get_io_context(), endpoints);
	
	qRegisterMetaType<filestruct::block>("filestruct::block");
	qRegisterMetaType<std::string>("std::string");
	qRegisterMetaType<QTextCursor>("QTextCursor");
	QMetaObject::Connection connecthanndle_pro_bar = connect(down_json_ptr_.get(), SIGNAL(sign_pro_bar(int, int)), this, SLOT(show_progress_bar(int, int)), Qt::QueuedConnection);
	if (connecthanndle_pro_bar)
	{
		OutputDebugString(L"pro bar 信号与槽函数关联成功\n");
		ui.text_log->insertPlainText(u8"pro_bar 槽函数关联成功\n");

	}
	else
	{
		OutputDebugString(L"pro bar 关联失败\n");
	}
	QMetaObject::Connection connecthanndle_name = connect(down_json_ptr_.get(), SIGNAL(sign_file_name(QString)), this, SLOT(show_file_name(QString)), Qt::QueuedConnection);
	if (connecthanndle_name)
	{
		OutputDebugString(L"name 信号与槽函数关联成功\n");
		ui.text_log->insertPlainText(u8"name 槽函数关联成功\n");

	}
	else
	{
		OutputDebugString(L"name 关联失败\n");
	}
	QMetaObject::Connection connecthanndle_log = connect(down_json_ptr_.get(), SIGNAL(sign_text_log(QString)), this, SLOT(show_text_log(QString)), Qt::QueuedConnection);
	if (connecthanndle_log)
	{
		OutputDebugString(L"text 信号与槽函数关联成功\n");
		ui.text_log->insertPlainText(u8"text_log 槽函数关联成功\n");

	}
	else
	{
		OutputDebugString(L"text_log 关联失败\n");
	}

	QMetaObject::Connection connect_block_ = connect(down_json_ptr_.get(), SIGNAL(sign_down_block(QVariant,QString, QString)), this,
		SLOT(down_block_file_(QVariant,QString, QString)), Qt::DirectConnection);
	if (connect_block_)
	{
		ui.text_log->insertPlainText(u8"block 关联成功\n");
	}
	QMetaObject::Connection connect_connect_ = connect(down_json_ptr_.get(), SIGNAL(signal_connect()), this,
		SLOT(show_connect()), Qt::QueuedConnection);
	if (connect_connect_)
	{
		ui.text_log->insertPlainText(u8"connect 关联成功\n");
	}
}


void client_page::down_block_file_(QVariant file_names,QString loadip, QString loadport)
{
	asio::ip::tcp::resolver resolver(io_pool_.get_io_context());

	auto endpoints = resolver.resolve("127.0.0.1", "12314");
	bck = file_names.value<filestruct::block>();

	down_block_ptr_ = std::make_shared<down_block_client>(io_pool_.get_io_context(), endpoints, bck);

	down_block_ptr_->send_filename();

	//QMetaObject::Connection connecthanndle_pro_bar = connect(down_block_.get(), SIGNAL(signal_pro_bar(int, int)), this, SLOT(show_progress_bar(int, int)), Qt::QueuedConnection);
	//if (connecthanndle_pro_bar)
	//{
	//	ui.text_log->insertPlainText(u8"block _pro_bar 信号与槽 关联成功\n");
	//}
	//QMetaObject::Connection connecthanndle_name = connect(down_block_.get(), SIGNAL(signal_file_name_(QString)), this, SLOT(show_file_name(QString)), Qt::QueuedConnection);
	//if (connecthanndle_name)
	//{
	//	ui.text_log->insertPlainText(u8"block _file_name 信号与槽 关联成功\n");
	//}
}


void client_page::wget_c_file_()
{


	asio::ip::tcp::resolver resolver_1(io_pool_.get_io_context());
	auto endpoint_1 = resolver_1.resolve({ "127.0.0.1","12313" });
	m_wget_c_file_ = std::make_shared<wget_c_file_client>(io_pool_.get_io_context(), endpoint_1/*,this*/);
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
	ui.pro_bar->setMaximum(maxvalue_);  //最大值
	ui.pro_bar->setValue(value_);  //当前进度

	dpro = (ui.pro_bar->value() - ui.pro_bar->minimum()) * 100 / (ui.pro_bar->maximum() - ui.pro_bar->minimum());

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