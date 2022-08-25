#pragma once
#include <string>

struct request_header
{
	char name_[32];
	uint32_t length_;
	uint32_t sequence_;
	uint32_t totoal_;
};


class request
{
public:
	template<typename _Ty, std::size_t N>
	void to_bytes(std::array<_Ty, N>& arr)
	{
		std::memcpy(arr.data(), &header_, sizeof(request_header));

		std::memcpy(arr.data() + sizeof(request_header), body_.data(), body_.size());
	}

public:
	request_header header_;

	std::string body_;
};