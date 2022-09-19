#include "client_page.h"
#include <QprogressBar>
#include <QMetaObject>
#include <Qstring>
#include <QMetaType>
#include <QVariant>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <QStandardItemModel>
client_page::client_page(QWidget* parent)
	: QMainWindow(parent)
	, io_pool_(10)
{
	ui.setupUi(this);

	ui.listwidget->setHeaderLabel(u8"选择要下载的文件路径");

	ui.pro_bar->setOrientation(Qt::Horizontal);  
	ui.pro_bar->setMinimum(0); 
	ui.pro_bar->setMaximum(100);


	qRegisterMetaType<QVariant>("QVariant");
	qRegisterMetaType<QTextCursor>("QTextCursor");
	qRegisterMetaType<std::size_t>("std:::size_t");

	connect(ui.init, &QPushButton::clicked, this, &client_page::init_listview);
	connect(ui.connect, &QPushButton::clicked, this, &client_page::request_connect);
	connect(ui.confirm, &QPushButton::clicked, this, &client_page::show_confirm);

	connect(ui.flush, &QPushButton::clicked, this, &client_page::show_flush_dir);

//	QMetaObject::Connection connecthanndle_pro_bar_ = connect(ui.listwidget,&QTreeWidget::itemPressed, this, &client_page::show_confirm);

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

	QMetaObject::Connection connecthanndle_pro_bar = connect(down_json_ptr_, SIGNAL(sign_pro_bar(int,int)), this, SLOT(show_progress_bar(int,int)), Qt::QueuedConnection);

	QMetaObject::Connection connecthanndle_name = connect(down_json_ptr_, SIGNAL(sign_file_name(QString)), this, SLOT(show_file_name(QString)), Qt::QueuedConnection);

	QMetaObject::Connection connecthanndle_log = connect(down_json_ptr_, SIGNAL(sign_text_log(QString)), this, SLOT(show_text_log(QString)), Qt::QueuedConnection);

	auto connect_block_ = QObject::connect(down_json_ptr_, &down_json_client::sign_down_block, [this](QVariant var, int id, QString ip, QString port)
							{
								this->down_block_file_(var, id, ip, port);
							});

}

void client_page::down_block_file_(QVariant file_names, int id, QString loadip, QString loadport)
{
	asio::ip::tcp::resolver resolver(io_pool_.get_io_context());

	auto endpoints = resolver.resolve("127.0.0.1", "12314");
	bck = file_names.value<filestruct::block>();

	auto down_block_ptr_ = std::make_shared<down_block_client>(io_pool_.get_io_context(), endpoints, bck);


	QMetaObject::Connection connecthanndle_ = connect(down_block_ptr_.get(), SIGNAL(signal_get_id_port_externl(std::size_t, QString)), SLOT(send_get_id_port_for_server(std::size_t, QString)), Qt::DirectConnection);
	
	QMetaObject::Connection connecthanndle_wget = connect(down_block_ptr_.get(), SIGNAL(signal_wget_down_file(QString)), SLOT(wget_c_file_(QString)), Qt::DirectConnection);

	QMetaObject::Connection connecthanndle_pro_bar = connect(down_block_ptr_.get(), SIGNAL(signal_pro_bar(int,int)), this, SLOT(show_progress_bar(int,int)), Qt::QueuedConnection);

	QMetaObject::Connection connecthanndle_name = connect(down_block_ptr_.get(), SIGNAL(signal_file_name_(QString)), this, SLOT(show_file_name(QString)), Qt::QueuedConnection);

	QMetaObject::Connection connecthanndle_text = connect(down_block_ptr_.get(), SIGNAL(signal_text_log(QString)), this, SLOT(show_text_log(QString)), Qt::QueuedConnection);



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

	QMetaObject::Connection connecthanndle_pro_bar = connect(wget_c_file_client_ptr_, SIGNAL(sign_wget_c_file_pro_bar(int, int)), this, SLOT(show_progress_bar(int, int)), Qt::QueuedConnection);

	QMetaObject::Connection connecthanndle_text_log = connect(wget_c_file_client_ptr_, SIGNAL(sign_wget_c_file_text_log(QString)), this, SLOT(show_text_log(QString)), Qt::QueuedConnection);

	QMetaObject::Connection connecthanndle_name = connect(wget_c_file_client_ptr_, SIGNAL(sign_wget_c_file_name(QString)), this, SLOT(show_file_name(QString)), Qt::QueuedConnection);

	std::string file_name = wget_file_name.toStdString();

	wget_c_file_client_ptr_->do_send_wget_file_name_text(file_name);
}

void client_page::show_progress_bar(int maxvalue_, int value_)
{

	double dpro = (value_ * 100.00) / maxvalue_;

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

void client_page::send_get_id_port_for_server(std::size_t get_server_id, QString get_server_port)
{

	std::string str = get_server_port.toStdString();
	std::string id_port = std::to_string(get_server_id)+','+str;
	down_json_ptr_->send_id_port(get_server_id,str);

}

void client_page::init_listview()
{
	ui.init->setEnabled(false);

	show_list_dir();

}

void client_page::show_flush_dir()
{

	ui.listwidget->clear();

	show_list_dir();
}

void client_page::show_list_dir()
{
	QString qstr;
	std::string str;
	std::string child_str;

	std::string file_path;
	std::vector<std::string> vstr;

	QTreeWidgetItem* item;
	QTreeWidgetItem* child ;

	QList<QTreeWidgetItem*> child_item;

	std::fstream list("list.json");

	if (!list.is_open())
		return;

	parse_client_list_json("list.json");
	parse_down_jsonfile("down.json");
	for (auto& iter : files_inclient.file_list)
	{
		child = new QTreeWidgetItem();

		item = new QTreeWidgetItem();

		file_path = downfile_path.path + "\\" + iter.path;

		auto pos = file_path.find_last_of("\\");

		str = file_path.substr(0, pos);
		qstr = QString::fromStdString(str);

		child_str = file_path.substr(pos+1);

		child_qstr = QString::fromStdString(child_str);


		item->setText(0, qstr);

		auto name = std::find_if(vstr.begin(), vstr.end(), [&](auto file) {return file == str; });
		if (name == vstr.end())
		{
			vstr.push_back(str);
		}	
		else 
		{
			continue;
		}

		child->setText(0, child_qstr);

		item->addChild(child);

		ui.listwidget->addTopLevelItem(item);

		child->setCheckState(0, Qt::Unchecked);

		item->setCheckState(0, Qt::Unchecked);


	}
}

void client_page::show_confirm()
{

	QTreeWidgetItemIterator it(ui.listwidget);

	while (*it) {

		if ((*it)->checkState(0) == Qt::Checked) {

			ui.listwidget->setCurrentItem(*it);

			QString text_ = (*it)->text(0) + "\n";

			ui.text_log->insertPlainText(text_);
		}
		++it;		
	}
}


void client_page::choose_down_filename()
{






}


