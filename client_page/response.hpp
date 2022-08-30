//client_ async_read
#pragma once
#include <array>
#include <cstdint>

struct response_header
{
	response_header(uint32_t id)
	{
		proto_id_ = id;
	}

	uint32_t proto_id_;
	char name_[32];
	uint32_t length_;
	uint32_t sequence_;
	uint32_t totoal_;
};


class response
{
public:
	template<typename _Ty, std::size_t N>
	void to_bytes(std::array<_Ty,N>& arr)
	{
		std::memcpy(arr.data(), &header_, sizeof(response_header));

		std::memcpy(arr.data() + sizeof(response_header), body_.data(), body_.size());
	}

public:
	response_header header_;

	std::array<char,8192> body_;
};