#pragma once
#include "asio.hpp"

class io_context_pool
{
public:
    explicit io_context_pool(std::size_t pool_size);

    void run();

    void stop();

    asio::io_context& get_io_context();
private:
    typedef std::shared_ptr<asio::io_context> io_context_ptr;
    typedef asio::executor_work_guard<asio::io_context::executor_type> io_context_work;

    std::vector<io_context_ptr> io_contexts_;
    std::list<io_context_work> work_;

    std::size_t next_io_context_;

};

