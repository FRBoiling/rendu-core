#pragma once

#include "common/define.h"
#include "common/log/formatter.h"
#include <memory>
#include <mutex>

BEGIN_NAMESPACE_COMMON
namespace log {

class Sink {
public:
    explicit Sink(Level level = Level::Info);
    virtual ~Sink() = default;

    void set_level(Level level);
    Level level() const;

    void set_formatter(std::shared_ptr<Formatter> formatter);

    void log(const LogMessage& msg);

protected:
    virtual void write(const std::string& formatted) = 0;

    bool should_log(Level level) const;

    Level level_;
    std::shared_ptr<Formatter> formatter_;
    mutable std::mutex mutex_;
};

} // namespace log
END_NAMESPACE_COMMON
