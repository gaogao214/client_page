#include "client_page.h"
#include <QprogressBar>
#include <QMetaObject>
#include <Qstring>
#include <QMetaType>
#include <QVariant>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <QStandardItemModel>
#include <QStringList>
#include <filesystem>


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
	connect(ui.confirm, &QPushButton::clicked, this, &client_page::selected_path);

	connect(ui.flush, &QPushButton::clicked, this, &client_page::show_flush_dir);

	QMetaObject::Connection connecthanndle_ = connect(ui.listwidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(itemChangedSlot(QTreeWidgetItem*, int)));

	//connect(ui.a_list_of, &QPushButton::clicked, this, &client_page::a_list_of);
	QMetaObject::Connection connect_ = connect(ui.a_list_of, SIGNAL(clicked(bool)), this, SLOT(a_list_of(bool)));

	ui.a_list_of->setCheckable(true);

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

	std::string child_str;
	QString child_qstr;

	QTreeWidgetItem* child_;

	std::set<std::string> vstr;

	std::string path;

	std::fstream list("list.json");

	if (!list.is_open())
		return;

	parse_client_list_json("list.json");
	parse_down_jsonfile("down.json");

	item = new QTreeWidgetItem();

	qstr = QString::fromStdString(downfile_path.path);

	item->setText(0, qstr);

	for (auto& iter : files_inclient.file_list)
	{
		child_ = new QTreeWidgetItem;

		auto pos = iter.path.find_last_of("\\");
		
		if (pos == std::string::npos)
		{
			child = new QTreeWidgetItem;

			QString q_path = QString::fromStdString(iter.path);

			child->setText(0, q_path);

			item->addChild(child);	
			
			child->setCheckState(0,Qt::Unchecked);
		}

		if(pos!=std::string::npos)
		{
			vstr.insert(path);	

			path = iter.path.substr(0, pos);

			auto path_name = std::find_if(vstr.begin(), vstr.end(), [&](auto file) {return file == path; });

			if (path == *vstr.begin())
			{
				child_str = iter.path.substr(pos + 1);

				child_qstr = QString::fromStdString(child_str);

				child_->setText(0, child_qstr);

				child->addChild(child_);
			
				child_->setCheckState(0,Qt::Unchecked);
			}
			else
			{
				child = new QTreeWidgetItem;

				QString q_path = QString::fromStdString(path);

				child->setText(0, q_path);

				item->addChild(child);

				child->setCheckState(0, Qt::Unchecked);

				child_str = iter.path.substr(pos + 1);

				child_qstr = QString::fromStdString(child_str);

				child_->setText(0, child_qstr);

				child->addChild(child_);

				child_->setCheckState(0, Qt::Unchecked);
			}
			
			if (*vstr.begin() == "")
				vstr.erase(vstr.begin());

		}	
	}
	ui.listwidget->addTopLevelItem(item);

	item->setCheckState(0, Qt::Unchecked);

	
}


void client_page::selected_path()
{
	std::vector<std::string> path_under_names;

	QString q_child_name_;
	QString q_grandchild_name_;
	std::string child_name_;

	QTreeWidgetItemIterator it(ui.listwidget);

	while (*it) {

		if ((*it)->checkState(0) == Qt::Checked) {

			ui.listwidget->setCurrentItem(*it);

			QString selected_path_ = (*it)->text(0);

			std::string str = selected_path_.toStdString();

			for (int i = 0; i < (*it)->childCount(); i++)
			{
				QTreeWidgetItem* path_under_child_ = (*it)->child(i);
				
				if (str == downfile_path.path)
				{
					q_child_name_ = path_under_child_->text(0);

					if (path_under_child_->childCount() > 0)
					{
						for (int j = 0; j < path_under_child_->childCount(); j++)
						{
							QTreeWidgetItem* path_under_grandchild_ = path_under_child_->child(j);
			
							q_grandchild_name_ = path_under_child_->text(0) + "\\" + path_under_grandchild_->text(0);

							std::string grandchild_name_ = q_grandchild_name_.toStdString();

							path_under_names.push_back(grandchild_name_);

						}
					}
					else
					{
						child_name_ = q_child_name_.toStdString();

						path_under_names.push_back(child_name_);
					}
				}
				else
				{
					q_child_name_ = selected_path_ + "\\" + path_under_child_->text(0);

					child_name_ = q_child_name_.toStdString();

					path_under_names.push_back(child_name_);

				}

				OutputDebugStringA(child_name_.data());
				OutputDebugStringA("\n");


				ui.text_log->insertPlainText(q_child_name_);
			}
		}
		++it;		
	}

	choose_down_names(path_under_names);

}


void client_page::choose_down_names(std::vector<std::string> text_)
{	
	QVariant var;

	parse_block_json_id("id.json");

	for (auto& iter : files_inclient.file_list)
	{
		auto down_name = std::find_if(text_.begin(), text_.end(), [&](auto name) {return name == iter.path; });
		if (down_name == text_.end())
			continue;
		
		id_index_[iter.blockid] += 1;
	}
	for (auto& iter : files_inclient.file_list)
	{
		auto down_name = std::find_if(text_.begin(), text_.end(), [&](auto name) {return name==iter.path; });
		if (down_name == text_.end())
			continue;

		if (down_name != text_.end())
		{
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

			if(id_index_[iter.blockid]==index_[iter.blockid])
			{
					var.setValue(choose_blks.blocks_[iter.blockid]);

					down_block_file_(var, iter.blockid, it->second.server.back().ip.data(), it->second.server.back().port.data());
			}

		}			
	}
}

void client_page::setChildCheckState(QTreeWidgetItem* item, Qt::CheckState cs)
{
	if (!item) return;
	for (int i = 0; i < item->childCount(); i++)
	{
		QTreeWidgetItem* child = item->child(i);
		if (child->checkState(0) != cs)
		{
			child->setCheckState(0, cs);
		}
	}
	setParentCheckState(item->parent());
}

void client_page::setParentCheckState(QTreeWidgetItem* item)
{
	if (!item) return;
	int selectedCount = 0;
	int childCount = item->childCount();
	for (int i = 0; i < childCount; i++)
	{
		QTreeWidgetItem* child = item->child(i);
		if (child->checkState(0) == Qt::Checked)
		{
			selectedCount++;
		}
	}

	if (selectedCount == 0) {
		item->setCheckState(0, Qt::Unchecked);
	}
	else if (selectedCount == childCount) {
		item->setCheckState(0, Qt::Checked);
	}
	else {
		item->setCheckState(0, Qt::PartiallyChecked);
	}

}


void client_page::itemChangedSlot(QTreeWidgetItem* item, int column)
{
	if (Qt::PartiallyChecked != item->checkState(0))
		setChildCheckState(item, item->checkState(0));

	if (Qt::PartiallyChecked == item->checkState(0))
		if (!isTopItem(item))
			item->parent()->setCheckState(0, Qt::PartiallyChecked);
}


bool client_page::isTopItem(QTreeWidgetItem* item)
{
	if (!item) return false;
	if (!item->parent()) return true;
	return false;
}

void client_page::a_list_of(bool checked)
{
	ui.listwidget->setItemsExpandable(true);

	ui.listwidget->expandAll();
}