#include "io_context_pool.h"
#include <thread>
io_context_pool::io_context_pool(std::size_t pool_size)
	:next_io_context_(0)
{
	if (pool_size == 0)
	{
		throw std::runtime_error("io_context_pool size is 0");
	}

	for (std::size_t i = 0; i < pool_size; i++)
	{
		io_context_ptr io_context(new asio::io_context);
		io_contexts_.push_back(io_context);
		work_.push_back(asio::make_work_guard(*io_context));
	}
}


void io_context_pool::run()
{
	std::vector<std::shared_ptr<std::thread>> threads;

	for (auto& iter : io_contexts_)
	{
		std::shared_ptr<std::thread> thread(new std::thread([&] { iter->run(); }));
		threads.push_back(thread);
	}

	for (auto& iter : threads)
	{
		iter->join();
	}
}

void io_context_pool::stop()
{
	for (auto& iter : io_contexts_)
	{
		iter->stop();
	}
}

asio::io_context& io_context_pool::get_io_context()
{
	asio::io_context& io_context = *io_contexts_[next_io_context_];
	++next_io_context_;
	if (next_io_context_ == io_contexts_.size())
		next_io_context_ = 0;

	return io_context;


}