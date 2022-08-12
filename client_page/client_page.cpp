#include "client_page.h"
#include "asio.hpp"
#include "down_json.h"
#include <QprogressBar>
#include <QMetaObject>
#include <Qstring>
#include "wget_c_file.h"
#include "down_block.h"
client_page::client_page(QWidget* parent)
	: QMainWindow(parent)
	//, main_thread_ptr_(new std::thread())
	, connect_ptr_(new std::thread())
{
	ui.setupUi(this);

	ui.pro_bar->setOrientation(Qt::Horizontal);  //进度条水平方向
	ui.pro_bar->setMinimum(0);  //最小值

	connect(ui.connect, &QPushButton::clicked, this, &client_page::request_connect);
	connect(ui.go_on, &QPushButton::clicked, this, &client_page::wget_c_file_);
	
	//清理本地文件
}

client_page::~client_page()
{
		
}

void client_page::request_connect()
{

	try
	{
		connect_ptr_.reset(new std::thread([this]()
			{
				asio::ip::tcp::resolver resolver(io_context);
				auto endpoints = resolver.resolve("127.0.0.1", "12312");

				p_ = std::make_shared<down_json>(io_context, endpoints/*, this*/);
				/*thread_ = new QThread();
				p_->moveToThread(thread_);*/
				
				QMetaObject::Connection connecthanndle_pro_bar = connect(p_.get(), SIGNAL(sign_pro_bar(int ,int )), this, SLOT(show_progress_bar(int ,int)), Qt::QueuedConnection);
				if (connecthanndle_pro_bar)
				{
					OutputDebugString(L"pro bar槽函数关联成功\n");
					ui.text_log->insertPlainText(u8"pro_bar 槽函数关联成功\n");

				}
				QMetaObject::Connection connecthanndle_name = connect(p_.get(), SIGNAL(sign_file_name(QString)), this, SLOT(show_file_name(QString)), Qt::QueuedConnection);
				if (connecthanndle_name)
				{
					OutputDebugString(L"name 槽函数关联成功\n");
					ui.text_log->insertPlainText(u8"name 槽函数关联成功\n");

				}
				QMetaObject::Connection connecthanndle_log = connect(p_.get(), SIGNAL(sign_text_log(QString)), this, SLOT(show_text_log(QString)), Qt::QueuedConnection);
				if (connecthanndle_log)
				{
					OutputDebugString(L"text 槽函数关联成功\n");
					ui.text_log->insertPlainText(u8"text_log 槽函数关联成功\n");

				}
				//thread_->start();
		/*connect_ptr_.reset(new std::thread([this]()
			{*/
				p_->run();
			}));

		connect_ptr_->detach();
	//	connect_ptr_->join();

	}
	catch (...)
	{
		OutputDebugString(L"s出现异常\n");

	}
	
	/*while (true)
	{
		auto future = p_->complete_process_.get_future();

		future.wait();
		
		ui.pro_bar->setValue(future.get());
		break;
	}*/


	//auto f = [this]()
	//{
	//	ui.text_log->insertPlainText(u8"端口 12312 连接成功\n");
	//};

	//connect_ptr_.reset(new std::thread([this,f]()
	//	{
	//		asio::ip::tcp::resolver resolver(io_context);
	//		auto endpoints = resolver.resolve("127.0.0.1", "12312");
	//		p_ = std::make_shared<down_json>(io_context, endpoints, this, f);
	//		p_->run();
	//	}));
	//connect_ptr_->detach();
}


void client_page::wget_c_file_()
{

	std::thread t1([this]()
		{
			asio::io_context ios_;
			asio::ip::tcp::resolver resolver_1(ios_);
			auto endpoint_1 = resolver_1.resolve({ "127.0.0.1","12313" });
			wget_c_file wcf(ios_, endpoint_1/*,this*/);
			ios_.run();
		});
	t1.detach();

}

void client_page::show_progress_bar( int maxvalue_ ,int value_)
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