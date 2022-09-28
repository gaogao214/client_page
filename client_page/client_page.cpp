#include "client_page.h"
#include <QprogressBar>
#include <QMetaObject>
#include <Qstring>
#include <QMetaType>
#include <QVariant>
#include <QListWidgetItem>
#include <QStandardItemModel>
#include <QStringList>
#include <filesystem>
#include <QInputDialog>
#include <QCheckBox>
#include <QFile>
#include <QTextStream>
#include <QDir>
client_page::client_page(QWidget* parent)
	: QMainWindow(parent)
	, io_pool_(10)
{
	ui.setupUi(this);

	init_checkedbox();

	ui.pro_bar->setOrientation(Qt::Horizontal);  
	ui.pro_bar->setMinimum(0); 
	ui.pro_bar->setMaximum(100);


	qRegisterMetaType<QVariant>("QVariant");
	qRegisterMetaType<QTextCursor>("QTextCursor");
	qRegisterMetaType<std::size_t>("std:::size_t");

	connect(ui.add_lose, SIGNAL(clicked(bool)), this, SLOT(add_lose_sight_of_listview(bool)));

	connect(ui.add_upload,SIGNAL(clicked(bool)),this,SLOT(add_upload_listview(bool)));

	connect(ui.connect, &QPushButton::clicked, this, &client_page::request_connect);

	connect(ui.clear_log, &QPushButton::clicked, this, &client_page::clear_log_);

	connect(ui.delete_lose,&QPushButton::clicked,this,&client_page::delete_lose_signt_of_listwidget);

	connect(ui.delete_upload, &QPushButton::clicked, this, &client_page::delete_upload_listwidget);

	connect(ui.clear_lose,&QPushButton::clicked,this,&client_page::clear_lose_signt_of_listwidget);

	connect(ui.clear_upload,&QPushButton::clicked,this,&client_page::clear_upload_listwidget);

	connect(ui.start_upload,&QPushButton::clicked,this,&client_page::start_upload_file);

	start_io_pool();
}

client_page::~client_page()
{
	stop();
}

void client_page::init_checkedbox()
{
	QFile state_lose_signt_of_file("E:\\Object_gao\\File_up_download\\qt_object\\client_page\\client_page\\save_lose_signt_of_path.txt");
	
	state_lose_signt_of_file.open(QIODevice::ReadOnly);

	QTextStream out(&state_lose_signt_of_file);
	QList<QString> str_list;

	while (!out.atEnd())
	{
		str_list.append(out.readLine());
	}
	
	for (auto& iter : str_list)
	{
		QListWidgetItem* item = new QListWidgetItem(iter);
		item->setCheckState(Qt::Unchecked);
		ui.listwidget_lose->addItem(item);

		ui.text_log->insertPlainText(iter);
	}

	QFile state_forced_upload_file("E:\\Object_gao\\File_up_download\\qt_object\\client_page\\client_page\\save_forced_upload_path.txt");

	state_forced_upload_file.open(QIODevice::ReadOnly);

	QTextStream out_(&state_forced_upload_file);
	QList<QString> strlist;

	while (!out_.atEnd())
	{
		strlist.append(out_.readLine());
	}

	for (auto& iter_ : strlist)
	{
		QListWidgetItem* item_ = new QListWidgetItem(iter_);
		item_->setCheckState(Qt::Unchecked);
		ui.listwidget_upload->addItem(item_);

		ui.text_log->insertPlainText(iter_);
	}

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

void client_page::save_lose_sight_of_path()
{
	QFile* fp = new QFile;
	fp->setFileName("save_lose_signt_of_path.txt");
	QDir::setCurrent("E:\\Object_gao\\File_up_download\\qt_object\\client_page\\client_page\\");
	bool ok = fp->open(QIODevice::WriteOnly);

	if (ok)
	{
		QTextStream out(fp);

		for (int i = 0; i < ui.listwidget_lose->count(); i++)
		{
			out << ui.listwidget_lose->item(i)->text() << endl;
		}
		fp->close();
	}
}

void client_page::save_forced_upload_path()
{

	QFile* fp = new QFile;
	fp->setFileName("save_forced_upload_path.txt");
	QDir::setCurrent("E:\\Object_gao\\File_up_download\\qt_object\\client_page\\client_page\\");
	bool ok = fp->open(QIODevice::WriteOnly);

	if (ok)
	{
		QTextStream out(fp);

		for (int i = 0; i < ui.listwidget_upload->count(); i++)
		{
			out << ui.listwidget_upload->item(i)->text() << endl;
		}
		fp->close();
	}
}

void client_page::add_lose_sight_of_listview(bool)
{

	bool ok ;
	QString text = QInputDialog::getText(this, u8"选择忽略路径", u8"请输入忽略路径", QLineEdit::Normal, 0, &ok, Qt::MSWindowsFixedSizeDialogHint);
	if (ok && !text.isEmpty())
	{
		QListWidgetItem* item = new QListWidgetItem(text);
		item->setCheckState(Qt::Unchecked);
		ui.listwidget_lose->addItem(item);

		save_lose_sight_of_path();

	}
}

void client_page::add_upload_listview(bool)
{

	bool ok;
	QString text = QInputDialog::getText(this, u8"选择强制更新路径", u8"请输入更新路径", QLineEdit::Normal, 0, &ok, Qt::MSWindowsFixedSizeDialogHint);
	if (ok && !text.isEmpty())
	{

		QListWidgetItem* item = new QListWidgetItem(text);
		item->setCheckState(Qt::Unchecked);
		ui.listwidget_upload->addItem(item);
		
		save_forced_upload_path();

	}
}

void client_page::delete_lose_signt_of_listwidget()
{
	int row = ui.listwidget_lose->count();
	if (row < 0)
		return;

	for (int i = 0; i < ui.listwidget_lose->count(); i++)
	{
		if (ui.listwidget_lose->item(i)->checkState() == Qt::Checked)
		{
			ui.listwidget_lose->takeItem(i);

			save_lose_sight_of_path();
		}
	}
}

void client_page::delete_upload_listwidget()
{
	int row = ui.listwidget_upload->count();
	if (row < 0)
		return;

	for (int i = 0; i < ui.listwidget_upload->count(); i++)
	{
		if (ui.listwidget_upload->item(i)->checkState() == Qt::Checked)
		{
			ui.listwidget_upload->takeItem(i);

			save_forced_upload_path();
		}
	}
}

void client_page::start_upload_file()
{
	parse_client_list_json("list.json");
	parse_down_jsonfile("down.json");
	
	selected_lose_signt_of_path();
	selected_forced_updating_path();
}

void client_page::selected_lose_signt_of_path()
{
	std::vector<std::string> lose_sight_of_vec_str;
	std::vector<std::string> lose_str;
	std::vector<std::string> lose_str_;
	std::vector<std::string> v_str_;
	int row = ui.listwidget_lose->count();
	if (row < 0)
		return;

	for (int i = 0; i < ui.listwidget_lose->count(); i++)
	{
		if (ui.listwidget_lose->item(i)->checkState() == Qt::Checked)
		{
			QString qstr = ui.listwidget_lose->item(i)->text();
			ui.text_log->insertPlainText(qstr);

			std::string str = qstr.toStdString();

			lose_str.push_back(str);

			
		}
	}


	for (auto& iter : files_inclient.file_list)
	{
		auto pos = iter.path.find_last_of("\\");

		auto name = iter.path.substr(0, pos);

		auto file_ = std::find_if(lose_str.begin(), lose_str.end(), [&](auto file) { return file == name; });

		if (file_ != lose_str.end()) //忽略的名字
		//if (str == name)
		{
			lose_sight_of_vec_str.push_back(iter.path);
		}
	}

	for (int i = 0; i < ui.listwidget_lose->count(); i++)
	{
		if (ui.listwidget_lose->item(i)->checkState() == Qt::Checked)
		{
			for (auto& iter : files_inclient.file_list)
			{
				auto name_ = std::find_if(lose_sight_of_vec_str.begin(), lose_sight_of_vec_str.end(), [&](auto file) {return file == iter.path; });
				if (name_ == lose_sight_of_vec_str.end())
				{
					lose_str_.push_back(iter.path);
				}
			}
		}
	}
	forced_updating_path(lose_str_);
}

void client_page::selected_forced_updating_path()
{
	std::vector<std::string> vec_str;

	int row = ui.listwidget_upload->count();
	if (row < 0)
		return;


	for (int i = 0; i < ui.listwidget_upload->count(); i++)
	{
		if (ui.listwidget_upload->item(i)->checkState() == Qt::Checked)
		{
			QString qstr = ui.listwidget_upload->item(i)->text();
			ui.text_log->insertPlainText(qstr);

			std::string str = qstr.toStdString();

			for (auto& iter : files_inclient.file_list)
			{
				auto pos = iter.path.find_last_of("\\");
				auto path_ = iter.path.substr(0, pos);
				if (path_ == str)
				{
					vec_str.push_back(iter.path);
				}
			}
			
		}
	}

	forced_updating_path(vec_str);


}

void client_page::forced_updating_path(std::vector<std::string> path)
{
	parse_block_json_id("id.json");

	QVariant var;

	if (path.empty())
		return;

	for (auto& iter : files_inclient.file_list)
	{
		auto down_name = std::find_if(path.begin(), path.end(), [&](auto name) {return name == iter.path; });
		if (down_name == path.end())
			continue;

		id_index_[iter.blockid] += 1;
	}

	for (auto& iter : files_inclient.file_list)
	{
		auto down_name = std::find_if(path.begin(), path.end(), [&](auto name) {return name == iter.path; });
		if (down_name == path.end())
			continue;

			choose_blks.blocks_[iter.blockid].id = iter.blockid;
			choose_blks.blocks_[iter.blockid].files.push_back(iter.path);

			std::string path_filename = downfile_path.path + "\\" + iter.path;

			std::filesystem::remove(path_filename);
			OutputDebugStringA(path_filename.data());
			OutputDebugStringA("\n");


			auto it = blks_.blocks.find(choose_blks.blocks_[iter.blockid].id);

			if (it == blks_.blocks.end())
				continue;

			int num_ = choose_blks.blocks_.size();

			index_[iter.blockid] += 1;

			if (id_index_[iter.blockid] == index_[iter.blockid])
			{
				var.setValue(choose_blks.blocks_[iter.blockid]);

				down_block_file_(var, iter.blockid, it->second.server.back().ip.data(), it->second.server.back().port.data());
			}
	}
	choose_blks.blocks_.clear();
}

void client_page::clear_log_()
{
	ui.text_log->clear();
}

void client_page::clear_lose_signt_of_listwidget()
{
	ui.listwidget_lose->clear();
	save_lose_sight_of_path();
}

void client_page::clear_upload_listwidget()
{
	ui.listwidget_upload->clear();
	save_forced_upload_path();
}