#include <common/log/sink.h>

BEGIN_NAMESPACE_COMMON
namespace log {

Sink::Sink(Level level)
    : level_(level)
    , formatter_(std::make_shared<DefaultFormatter>())
{
}

void Sink::set_level(Level level) {
    std::lock_guard<std::mutex> lock(mutex_);
    level_ = level;
}

Level Sink::level() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return level_;
}

void Sink::set_formatter(std::shared_ptr<Formatter> formatter) {
    std::lock_guard<std::mutex> lock(mutex_);
    formatter_ = std::move(formatter);
}

void Sink::log(const LogMessage& msg) {
    if (!should_log(msg.level)) {
        return;
    }

    std::string formatted;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        formatted = formatter_->format(msg);
    }

    write(formatted);
}

bool Sink::should_log(Level level) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(level_);
}

} // namespace log
END_NAMESPACE_COMMON
