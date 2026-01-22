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

BEGIN_NAMESPACE_COMMON
namespace log {

// 前向声明
class IoContext;

// 异步日志记录器类（基于 io_context）
class Logger {
public:
    explicit Logger(std::string name, IoContext& io);
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
    IoContext& io_context();

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
    IoContext& io_;
    Level level_;
    std::vector<std::shared_ptr<Sink>> sinks_;
    mutable std::mutex mutex_;
};

// 全局默认 Logger
Logger& default_logger();

// 获取/创建命名 Logger
Logger& get_logger(const std::string& name);
void set_default_logger(std::shared_ptr<Logger> logger);

// 宏定义
#define RENDU_LOG_TRACE(msg) ::rendu::log::default_logger().trace(msg)
#define RENDU_LOG_DEBUG(msg) ::rendu::log::default_logger().debug(msg)
#define RENDU_LOG_INFO(msg)  ::rendu::log::default_logger().info(msg)
#define RENDU_LOG_WARN(msg)  ::rendu::log::default_logger().warn(msg)
#define RENDU_LOG_ERROR(msg) ::rendu::log::default_logger().error(msg)
#define RENDU_LOG_CRITICAL(msg) ::rendu::log::default_logger().critical(msg)

#define RENDU_NAMED_LOG(name, level, msg) \
    ::rendu::log::get_logger(name).log(level, msg)

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
namespace rendu::log {

template<typename... Args>
void Logger::log_fields(Level level, const std::string& msg, Args&&... args) {
    static_assert(sizeof...(args) % 2 == 0, "Number of arguments must be even (key-value pairs)");

    std::map<std::string, std::string> fields;
    auto insert_field = [&fields](auto&& key, auto&& value) {
        std::ostringstream oss;
        oss << value;
        fields[std::forward<decltype(key)>(key)] = oss.str();
    };

    (insert_field(args), ...);

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

} // namespace rendu::log
