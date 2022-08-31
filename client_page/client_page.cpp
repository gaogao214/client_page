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

	ui.pro_bar->setOrientation(Qt::Horizontal);  //������ˮƽ����
	ui.pro_bar->setMinimum(0);  //��Сֵ
	qRegisterMetaType<QVariant>("QVariant");
	connect(ui.connect, &QPushButton::clicked, this, &client_page::request_connect);
	connect(ui.go_on, &QPushButton::clicked, this, &client_page::wget_c_file_);
	//�������ļ�

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
	QMetaObject::Connection connecthanndle_pro_bar = connect(down_json_ptr_, SIGNAL(sign_pro_bar(int, int)), this, SLOT(show_progress_bar(int, int)), Qt::QueuedConnection);
	if (connecthanndle_pro_bar)
	{
		OutputDebugString(L"pro bar �ź���ۺ��������ɹ�\n");
		ui.text_log->insertPlainText(u8"pro_bar �ۺ��������ɹ�\n");

	}
	else
	{
		OutputDebugString(L"pro bar ����ʧ��\n");
	}
	//QMetaObject::Connection connecthanndle_name = connect(down_json_ptr_, SIGNAL(sign_file_name(QString)), this, SLOT(show_file_name(QString)), Qt::QueuedConnection);
	//if (connecthanndle_name)
	//{
	//	OutputDebugString(L"name �ź���ۺ��������ɹ�\n");
	//	ui.text_log->insertPlainText(u8"name �ۺ��������ɹ�\n");

	//}
	//else
	//{
	//	OutputDebugString(L"name ����ʧ��\n");
	//}
	//QMetaObject::Connection connecthanndle_log = connect(down_json_ptr_, SIGNAL(sign_text_log(QString)), this, SLOT(show_text_log(QString)), Qt::QueuedConnection);
	//if (connecthanndle_log)
	//{
	//	OutputDebugString(L"text �ź���ۺ��������ɹ�\n");
	//	ui.text_log->insertPlainText(u8"text_log �ۺ��������ɹ�\n");

	//}
	//else
	//{
	//	OutputDebugString(L"text_log ����ʧ��\n");
	//}

	//QMetaObject::Connection connect_block_ = connect(down_json_ptr_, SIGNAL(sign_down_block(QVariant,QString, QString)), this,
	//	SLOT(down_block_file_(QVariant,QString, QString)), Qt::QueuedConnection);


	auto connect_block_ = QObject::connect(down_json_ptr_, &down_json_client::sign_down_block, [this](QVariant var, QString ip, QString port)
		{
			this->down_block_file_(var, ip, port);
			ui.text_log->insertPlainText(u8"block �����ɹ�\n");
		});


	//QMetaObject::Connection connect_connect_ = connect(down_json_ptr_, SIGNAL(signal_connect()), this,
	//	SLOT(show_connect()), Qt::QueuedConnection);
	//if (connect_connect_)
	//{
	//	ui.text_log->insertPlainText(u8"connect �����ɹ�\n");
	//}
}


void client_page::down_block_file_(QVariant file_names,QString loadip, QString loadport)
{
	asio::ip::tcp::resolver resolver(io_pool_.get_io_context());

	auto endpoints = resolver.resolve("127.0.0.1", "12314");
	bck = file_names.value<filestruct::block>();

	//if(down_block_ptr_==nullptr)
	auto down_block_ptr_ = std::make_shared<down_block_client>(io_pool_.get_io_context(), endpoints, bck);
	


	qRegisterMetaType<std::size_t>("std:::size_t");

	QMetaObject::Connection connecthanndle_ = connect(down_block_ptr_.get(), SIGNAL(signal_get_id_port_externl(std::size_t, QString)), SLOT(send_get_id_port_for_server(std::size_t, QString)), Qt::DirectConnection);
	if (connecthanndle_)
	{
		ui.text_log->insertPlainText(u8"�ͻ���ת������ �ź���� �����ɹ�\n");
	}







down_block_ptr_->send_filename();

	down_blocks_.push_back(down_block_ptr_);


	/*QMetaObject::Connection connecthanndle_pro_bar = connect(down_block_ptr_.get(), SIGNAL(signal_pro_bar(int, int)), this, SLOT(show_progress_bar(int, int)), Qt::QueuedConnection);
	if (connecthanndle_pro_bar)
	{
		ui.text_log->insertPlainText(u8"block _pro_bar �ź���� �����ɹ�\n");
	}*/
	//QMetaObject::Connection connecthanndle_name = connect(down_block_.get(), SIGNAL(signal_file_name_(QString)), this, SLOT(show_file_name(QString)), Qt::QueuedConnection);
	//if (connecthanndle_name)
	//{
	//	ui.text_log->insertPlainText(u8"block _file_name �ź���� �����ɹ�\n");
	//}
}


void client_page::wget_c_file_()
{

	asio::ip::tcp::resolver resolver_(io_pool_.get_io_context());
	auto endpoint_ = resolver_.resolve( "127.0.0.1","12313" );
	m_wget_c_file_ = std::make_shared<wget_c_file_client>(io_pool_.get_io_context(), endpoint_/*,this*/);
	m_wget_c_file_->do_send_wget_file_name_text();
	//QMetaObject::Connection connecthanndle_pro_bar = connect(m_wget_c_file_.get(), SIGNAL(sign_wget_c_file_pro_bar(int, int)), this, SLOT(show_progress_bar(int, int)), Qt::QueuedConnection);
	//if (connecthanndle_pro_bar)
	//{
	//	ui.text_log->insertPlainText(u8"wget_c_file_pro_bar �ź���۹����ɹ�\n");
	//}
	//else
	//{
	//	OutputDebugString(L"wget_pro_bar ����ʧ��\n");
	//}
	//QMetaObject::Connection connecthanndle_name = connect(m_wget_c_file_.get(), SIGNAL(sign_wget_c_file_name(QString)), this, SLOT(show_file_name(QString)), Qt::QueuedConnection);
	//if (connecthanndle_name)
	//{
	//	ui.text_log->insertPlainText(u8"wget_name �ۺ��������ɹ�\n");
	//}
	//else
	//{
	//	OutputDebugString(L"wget_name ����ʧ��\n");
	//}
	//QMetaObject::Connection connecthanndle_text_log = connect(m_wget_c_file_.get(), SIGNAL(sign_wget_c_file_text_log(QString)), this, SLOT(show_text_log(QString)), Qt::QueuedConnection);
	//if (connecthanndle_text_log)
	//{
	//	ui.text_log->insertPlainText(u8"wget_text_log �ۺ��������ɹ�\n");
	//}
	//else
	//{
	//	OutputDebugString(L"wget_text_log ����ʧ��\n");
	//}

}

void client_page::show_progress_bar(int maxvalue_, int value_)
{
	ui.pro_bar->setMaximum(maxvalue_);  //���ֵ
	ui.pro_bar->setValue(value_);  //��ǰ����

	dpro = (ui.pro_bar->value() - ui.pro_bar->minimum()) * 100 / (ui.pro_bar->maximum() - ui.pro_bar->minimum());

	ui.pro_bar->setFormat(QString::fromLocal8Bit("��ǰ����Ϊ:%1%").arg(QString::number(dpro, 'f', 1)));
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
	ui.text_log->insertPlainText(u8"���ӳɹ�");
}


void client_page::send_get_id_port_for_server(std::size_t get_server_id, QString get_server_port)
{

	OutputDebugString(L"�ͻ���ת������    �����________________________________________________\n");
	std::string str = get_server_port.toStdString();
	std::string id_port = std::to_string(get_server_id)+','+str;
	down_json_ptr_->send_id_port(/*id_port*/get_server_id,str);
}