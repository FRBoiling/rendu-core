#pragma once

#include "common/define.h"
#include "common/log/sink.h"

BEGIN_NAMESPACE_COMMON
namespace log {

class ConsoleSink : public Sink {
public:
    ConsoleSink(Level level = Level::Info);
    ~ConsoleSink() override = default;

protected:
    void write(const std::string& formatted) override;

private:
    std::string level_color(Level level);
    std::string reset_color();

    bool use_color_;
    bool is_terminal_;
};

} // namespace log
END_NAMESPACE_COMMON
