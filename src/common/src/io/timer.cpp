#include <common/io/timer.h>
#include <common/io/io_context.h>

BEGIN_NAMESPACE_COMMON
namespace io {

Timer::Timer(IoContext& io)
    : io_(io)
    , timer_(std::make_unique<boost::asio::steady_timer>(io.native()))
    , cancel_token_(std::make_shared<std::atomic<bool>>(false))
    , active_(false)
{
}

Timer::~Timer()
{
    // 不调用 cancel()，因为 Timer 可能被移动
    // 由析构的对象调用 cancel() 会影响移动后的对象
}

void Timer::expires_after(std::chrono::milliseconds delay, Callback callback)
{
    cancel(); // 取消之前的定时器

    cancel_token_->store(false);
    active_ = true;

    auto weak_token = std::weak_ptr<std::atomic<bool>>(cancel_token_);
    timer_->expires_after(delay);

    auto cb = [this, weak_token, callback = std::move(callback)](
        const boost::system::error_code& ec) {
        // 检查是否被取消
        auto token = weak_token.lock();
        if (!token || token->load()) {
            return;
        }

        if (!ec) {
            callback();
        }
        active_ = false;
    };

    timer_->async_wait(cb);
}

void Timer::expires_at(std::chrono::steady_clock::time_point time, Callback callback)
{
    cancel(); // 取消之前的定时器

    cancel_token_->store(false);
    active_ = true;

    auto weak_token = std::weak_ptr<std::atomic<bool>>(cancel_token_);
    timer_->expires_at(time);

    auto cb = [this, weak_token, callback = std::move(callback)](
        const boost::system::error_code& ec) {
        // 检查是否被取消
        auto token = weak_token.lock();
        if (!token || token->load()) {
            return;
        }

        if (!ec) {
            callback();
        }
        active_ = false;
    };

    timer_->async_wait(cb);
}

Timer::CancelToken Timer::repeat(std::chrono::milliseconds interval, Callback callback)
{
    cancel(); // 取消之前的定时器

    cancel_token_->store(false);
    active_ = true;

    auto weak_token = std::weak_ptr<std::atomic<bool>>(cancel_token_);

    // 递归函数实现周期性执行，使用 shared_ptr 延长生命周期
    auto schedule_next = std::make_shared<std::function<void()>>();
    *schedule_next = [this, interval, callback, weak_token, schedule_next]() {
        timer_->expires_after(interval);

        timer_->async_wait([this, interval, callback, weak_token, schedule_next](
            const boost::system::error_code& ec) {
            // 检查是否被取消
            auto token = weak_token.lock();
            if (!token || token->load()) {
                active_ = false;
                return;
            }

            if (!ec) {
                callback();
                (*schedule_next)(); // 继续下一次
            } else {
                active_ = false;
            }
        });
    };

    (*schedule_next)(); // 启动第一次执行
    return cancel_token_;
}

void Timer::cancel()
{
    if (this->cancel_token_) {
        this->cancel_token_->store(true);
    }
    if (this->timer_) {
        this->timer_->cancel();
    }
    this->active_ = false;
}

bool Timer::active() const
{
    return active_;
}

} // namespace io
END_NAMESPACE_COMMON
