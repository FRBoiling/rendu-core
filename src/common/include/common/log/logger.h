#pragma once

#include "common/define.h"
#include "common/log/sink.h"
#include "common/log/console_sink.h"
#include "common/io/io_context.h"
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <functional>
#include <fmt/core.h>

BEGIN_NAMESPACE_COMMON
namespace log {

// 异步日志记录器类（基于 io_context）
class Logger {
public:
    explicit Logger(std::string name, io::IoContext& io);
    ~Logger() = default;

    // 添加 Sink
    void add_sink(std::shared_ptr<Sink> sink);
    void remove_sink(std::shared_ptr<Sink> sink);
    void clear_sinks();

    // 设置日志级别
    void set_level(Level level);
    Level level() const;

    // 日志输出
    void log(Level level, const std::string& msg);

    // 便捷方法
    void trace(const std::string& msg);
    void debug(const std::string& msg);
    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);
    void critical(const std::string& msg);

    const std::string& name() const;
    io::IoContext& io_context();

    // 结构化日志（支持 key-value 字段）
    template<typename... Args>
    void log_fields(Level level, const std::string& msg, Args&&... args);

    template<typename... Args>
    void info_fields(const std::string& msg, Args&&... args);

    template<typename... Args>
    void debug_fields(const std::string& msg, Args&&... args);

    template<typename... Args>
    void warn_fields(const std::string& msg, Args&&... args);

    template<typename... Args>
    void error_fields(const std::string& msg, Args&&... args);

private:
    void async_log(Level level, const std::string& msg, const std::map<std::string, std::string>& fields = {});
    std::string get_timestamp();

    std::string name_;
    io::IoContext& io_;
    Level level_;
    std::vector<std::shared_ptr<Sink>> sinks_;
    mutable std::mutex mutex_;
};

// 全局默认 Logger
Logger& default_logger();

// 初始化默认 io_context
void init_default_io_context(io::IoContext& io);

// 获取/创建命名 Logger
Logger& get_logger(const std::string& name);
void set_default_logger(std::shared_ptr<Logger> logger);

// 日志宏（支持 fmt 风格的格式化）
#define RENDU_LOG_TRACE(...) ::Rendu::log::default_logger().trace(fmt::format(__VA_ARGS__))
#define RENDU_LOG_DEBUG(...) ::Rendu::log::default_logger().debug(fmt::format(__VA_ARGS__))
#define RENDU_LOG_INFO(...) ::Rendu::log::default_logger().info(fmt::format(__VA_ARGS__))
#define RENDU_LOG_WARN(...) ::Rendu::log::default_logger().warn(fmt::format(__VA_ARGS__))
#define RENDU_LOG_ERROR(...) ::Rendu::log::default_logger().error(fmt::format(__VA_ARGS__))
#define RENDU_LOG_CRITICAL(...) ::Rendu::log::default_logger().critical(fmt::format(__VA_ARGS__))

#define RENDU_NAMED_LOG(name, level, msg) \
    ::Rendu::log::get_logger(name).log(level, msg)

// 流式日志宏
#define RENDU_LOG_STREAM(logger, level) \
    logger.log(logger.level()), std::ostringstream()

#define RENDU_STREAM(logger, level) \
    std::ostringstream(), [](std::ostringstream& oss) -> decltype(auto) { \
        std::string msg = oss.str(); \
        logger.log(level, msg); \
        return std::move(msg); \
    }

} // namespace log
END_NAMESPACE_COMMON

// 模板实现
BEGIN_NAMESPACE_COMMON
namespace log {

template<typename... Args>
void Logger::log_fields(Level level, const std::string& msg, Args&&... args) {
    static_assert(sizeof...(args) % 2 == 0, "Number of arguments must be even (key-value pairs)");

    std::map<std::string, std::string> fields;

    // 使用辅助函数来处理 key-value 对
    auto process_pair = [&fields]<typename K, typename V>(K&& key, V&& value) {
        std::ostringstream oss;
        oss << std::forward<V>(value);
        fields[std::string(std::forward<K>(key))] = oss.str();
    };

    // 手动处理每一对
    std::tuple<Args...> args_tuple{std::forward<Args>(args)...};

    if constexpr (sizeof...(Args) >= 2) {
        process_pair(std::get<0>(args_tuple), std::get<1>(args_tuple));
    }
    if constexpr (sizeof...(Args) >= 4) {
        process_pair(std::get<2>(args_tuple), std::get<3>(args_tuple));
    }
    if constexpr (sizeof...(Args) >= 6) {
        process_pair(std::get<4>(args_tuple), std::get<5>(args_tuple));
    }
    if constexpr (sizeof...(Args) >= 8) {
        process_pair(std::get<6>(args_tuple), std::get<7>(args_tuple));
    }
    if constexpr (sizeof...(Args) >= 10) {
        process_pair(std::get<8>(args_tuple), std::get<9>(args_tuple));
    }

    async_log(level, msg, fields);
}

template<typename... Args>
void Logger::info_fields(const std::string& msg, Args&&... args) {
    log_fields(Level::Info, msg, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::debug_fields(const std::string& msg, Args&&... args) {
    log_fields(Level::Debug, msg, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::warn_fields(const std::string& msg, Args&&... args) {
    log_fields(Level::Warn, msg, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::error_fields(const std::string& msg, Args&&... args) {
    log_fields(Level::Error, msg, std::forward<Args>(args)...);
}

} // namespace log
END_NAMESPACE_COMMON
