#include <common/io/scheduler.h>
#include <memory>
#include <vector>

BEGIN_NAMESPACE_COMMON
namespace io {

Scheduler::Scheduler(IoContext& io)
    : io_(io)
{
}

void Scheduler::post(std::function<void()> task)
{
    io_.post(std::move(task));
}

Scheduler::CancelToken Scheduler::delayed(std::function<void()> task, std::chrono::milliseconds delay)
{
    auto timer = std::make_unique<Timer>(io_);

    // Timer::expires_after 不返回 token，我们创建一个用于 Scheduler::cancel
    auto token = std::make_shared<std::atomic<bool>>(false);

    // 包装回调以支持取消
    timer->expires_after(delay, [task = std::move(task), weak_token = std::weak_ptr<std::atomic<bool>>(token)]() {
        auto t = weak_token.lock();
        if (!t || t->load()) {
            return;
        }
        task();
    });

    timers_.push_back({std::move(timer), token});
    return token;
}

Scheduler::CancelToken Scheduler::periodic(std::function<void()> task, std::chrono::milliseconds interval)
{
    auto timer = std::make_unique<Timer>(io_);
    auto token = timer->repeat(interval, std::move(task));

    timers_.push_back({std::move(timer), token});
    return token;
}

void Scheduler::cancel(CancelToken token)
{
    if (token) {
        token->store(true);
        // 同时调用 Timer::cancel
        for (auto& task : timers_) {
            if (task.token == token && task.timer) {
                task.timer->cancel();
            }
        }
    }
}

} // namespace io
END_NAMESPACE_COMMON
