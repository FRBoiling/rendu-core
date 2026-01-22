#include <common/log/formatter.h>
#include <iomanip>
#include <sstream>
#include <regex>

BEGIN_NAMESPACE_COMMON
namespace log {

std::string DefaultFormatter::format(const LogMessage& msg) {
    std::ostringstream oss;
    oss << "[" << msg.timestamp << "] "
        << "[" << level_to_string(msg.level) << "] "
        << "[" << msg.logger_name << "] "
        << msg.message;

    // 添加结构化字段
    if (!msg.fields.empty()) {
        oss << " [";
        bool first = true;
        for (const auto& [key, value] : msg.fields) {
            if (!first) {
                oss << ", ";
            }
            oss << key << "=" << value;
            first = false;
        }
        oss << "]";
    }

    return oss.str();
}

std::string DefaultFormatter::level_to_string(Level level) {
    switch (level) {
        case Level::Trace:    return "TRACE";
        case Level::Debug:    return "DEBUG";
        case Level::Info:     return "INFO ";
        case Level::Warn:     return "WARN ";
        case Level::Error:    return "ERROR";
        case Level::Critical: return "CRITICAL";
        default:              return "UNKNOWN";
    }
}

PatternFormatter::PatternFormatter(const std::string& pattern)
    : pattern_(pattern)
{
}

std::string PatternFormatter::format(const LogMessage& msg) {
    std::string result = pattern_;

    // 替换占位符
    result = std::regex_replace(result, std::regex("%t"), msg.timestamp);
    result = std::regex_replace(result, std::regex("%l"), level_to_string(msg.level));
    result = std::regex_replace(result, std::regex("%n"), msg.logger_name);
    result = std::regex_replace(result, std::regex("%m"), msg.message);
    result = std::regex_replace(result, std::regex("%i"), thread_id_to_string(msg.thread_id));
    result = std::regex_replace(result, std::regex("%%"), "%");

    // 处理结构化字段 %f
    size_t pos = result.find("%f");
    if (pos != std::string::npos) {
        std::ostringstream fields_oss;
        if (!msg.fields.empty()) {
            bool first = true;
            for (const auto& [key, value] : msg.fields) {
                if (!first) {
                    fields_oss << " ";
                }
                fields_oss << key << "=" << value;
                first = false;
            }
        }
        result.replace(pos, 2, fields_oss.str());
    }

    return result;
}

std::string PatternFormatter::level_to_string(Level level) {
    switch (level) {
        case Level::Trace:    return "TRACE";
        case Level::Debug:    return "DEBUG";
        case Level::Info:     return "INFO";
        case Level::Warn:     return "WARN";
        case Level::Error:    return "ERROR";
        case Level::Critical: return "CRITICAL";
        default:              return "UNKNOWN";
    }
}

std::string PatternFormatter::thread_id_to_string(std::thread::id id) {
    std::ostringstream oss;
    oss << id;
    return oss.str();
}

} // namespace log
END_NAMESPACE_COMMON
