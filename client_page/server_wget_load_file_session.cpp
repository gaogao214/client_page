#include "server_wget_load_file_session.h"
#include "file_struct.h"
#include "request.hpp"
#include "response.hpp"

static constexpr std::size_t send_size = 8192;
static constexpr char down_name[32] = "down.json";

int server_wget_load_file_session::read_handle(uint32_t id)
{
	switch (id)
	{
	case request_number::name_text_request:

		name_text_request req;

		req.parse_bytes(buffer_);

		std::ofstream wget_c_file(req.body_.name_, std::ios::binary);
		wget_c_file.write(req.body_.text_, req.header_.length_);
		wget_c_file.close();

		parse_wget_c_file_json(req.header_.name_);

		send_file();
		break;
	}
	return 0;
}



void server_wget_load_file_session::send_file()
{
	size_t file_size = 0;

	size_t remaining_total = 0;

	std::size_t nleft_ = 0;

	parse_down_jsonfile(down_name);
	for (auto& iter : wcfi.wget_c_file_list)
	{
		auto wget_name = iter.wget_name;
		auto wget_offset = iter.offset;
		std::string file_path_name = downfile_path.path + "\\" + wget_name;

		file_size = get_file_len(file_path_name);

		std::ifstream file(file_path_name.c_str(), std::ios::in | std::ios::binary);
		if (!file.is_open())
			return;



		if (wget_offset < file_size)
		{
			remaining_total = file_size - wget_offset;

			if (remaining_total > send_size)
			{
				nchunkcount_ = remaining_total / send_size;

				if (remaining_total % nchunkcount_ != 0)
				{
					nchunkcount_++;
				}

				for (int i = 0; i < nchunkcount_; i++)
				{
					if (i + 1 == nchunkcount_)
					{
						nleft_ = remaining_total - send_size * (nchunkcount_ - 1);
					}
					else
					{
						nleft_ = send_size;
					}


					char* count_file_buf = new char[nleft_];

					std::size_t offset_ = i * send_size + wget_offset;
					file.seekg(offset_, std::ios::beg);
					file.read(count_file_buf, nleft_);

					offset_text_response resp;
					resp.header_.length_ = nleft_;
					resp.header_.totoal_ = nchunkcount_;
					std::memcpy(resp.header_.name_, wget_name.data(), wget_name.size());
					resp.body_.offset_ = offset_;
					resp.body_.set_text(count_file_buf);

					std::memset(count_file_buf, 0, nleft_);

					this->async_write(std::move(resp), [this](std::error_code ec, std::size_t sz)
						{
							if (ec)
								return;
						});
				}

			}
			else if (remaining_total < send_size)
			{
				remaining_total = file_size - wget_offset;
				char* count_file_buf = new char[remaining_total];
				file.read(count_file_buf, remaining_total);

				std::size_t total_num = 1;

				offset_text_response resp;
				resp.header_.length_ = remaining_total;
				resp.header_.totoal_ = total_num;
				std::memcpy(resp.header_.name_, wget_name.data(), wget_name.size());
				resp.body_.offset_ = wget_offset;
				resp.body_.set_text(count_file_buf);
				std::memset(count_file_buf, 0, remaining_total);

				this->async_write(std::move(resp));
			}

		}
	}
}







