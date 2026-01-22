#include <common/io/io_context.h>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/post.hpp>

BEGIN_NAMESPACE_COMMON
namespace io {

IoContext::IoContext(size_t thread_count)
    : running_(false)
    , thread_count_(thread_count)
{
    // 创建 work guard，防止 io_context 空闲时退出
    work_ = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(
        boost::asio::make_work_guard(io_context_)
    );
}

IoContext::~IoContext()
{
    stop();
}

void IoContext::post(std::function<void()> task)
{
    boost::asio::post(io_context_, std::move(task));
}

void IoContext::run()
{
    if (running_) {
        return;  // 已经在运行中
    }

    running_ = true;

    // 启动线程池
    threads_.reserve(thread_count_);
    for (size_t i = 0; i < thread_count_; ++i) {
        threads_.emplace_back([this]() {
            io_context_.run();
        });
    }
}

void IoContext::stop()
{
    // 停止 work guard
    if (work_) {
        work_.reset();
    }

    // 停止 io_context
    io_context_.stop();

    // 等待所有线程结束
    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    threads_.clear();

    running_ = false;
}

boost::asio::io_context& IoContext::native()
{
    return io_context_;
}

const boost::asio::io_context& IoContext::native() const
{
    return io_context_;
}

bool IoContext::running() const
{
    return running_;
}

} // namespace io
END_NAMESPACE_COMMON
