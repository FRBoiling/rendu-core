#include <common/log/console_sink.h>
#include <iostream>
#include <unistd.h>

BEGIN_NAMESPACE_COMMON
namespace log {

ConsoleSink::ConsoleSink(Level level)
    : Sink(level)
{
    // 检测是否是终端
    is_terminal_ = isatty(STDOUT_FILENO) == 1;
    // 终端支持颜色
    use_color_ = is_terminal_;
}

void ConsoleSink::write(const std::string& formatted) {
    if (use_color_) {
        std::cout << "\033[" << level_color(static_cast<Level>(level())) << "m"
                  << formatted << reset_color() << std::endl;
    } else {
        std::cout << formatted << std::endl;
    }
}

std::string ConsoleSink::level_color(Level level) {
    switch (level) {
        case Level::Trace:    return "37";   // White
        case Level::Debug:    return "36";   // Cyan
        case Level::Info:     return "32";   // Green
        case Level::Warn:     return "33";   // Yellow
        case Level::Error:    return "31";   // Red
        case Level::Critical: return "35";   // Magenta
        default:              return "0";    // Reset
    }
}

std::string ConsoleSink::reset_color() {
    return "\033[0m";
}

} // namespace log
END_NAMESPACE_COMMON
