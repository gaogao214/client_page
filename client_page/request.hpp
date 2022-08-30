//client   async_write
#pragma once
#include <string>

struct request_header
{
	request_header(uint32_t id)
	{
		proto_id_ = id;
	}

	uint32_t proto_id_;
	char name_[32];
	uint32_t length_;
	uint32_t sequence_;
	uint32_t totoal_;
};

template<typename _Body, uint32_t ID>
class request
{
public:
	request()
		: header_(ID)
	{

	}

public:
	template<typename _Ty, std::size_t N>
	void to_bytes(std::array<_Ty, N>& arr)
	{
		std::memcpy(arr.data(), &header_, sizeof(request_header));

		//std::memcpy(arr.data() + sizeof(request_header), body_.data(), body_.size());

		body_.to_bytes(arr.data() + sizeof(request_header));
	}

	template<typename _Ty, std::size_t N>
	void parse_bytes(std::array<_Ty, N>& arr)
	{
		std::memcpy(header_, arr.data(), sizeof(request_header));

		body_.parse_bytes(arr + sizeof(request_header));
	}

public:
	request_header header_;

	_Body body_;
};

struct id_port
{
	int id_;
	char port_[32];

	template<typename _Ty>
	void to_bytes(_Ty* arr)
	{
		std::memcpy(arr, &id_, sizeof(id_));

		std::memcpy(arr + sizeof(id_), port_, 32);
	}

	template<typename _Ty>
	void parse_bytes(_Ty* arr)
	{
		std::memcpy(id_, arr, sizeof(id_));

		std::memcpy(port_, arr+sizeof(id_), 32);
	}

	void set_port(const std::string& pt)
	{
		std::memcpy(port_, pt.data(), 32);
	}
};

using id_port_request = request<id_port,1000>;