#include <common/log/logger.h>
#include <common/log/sink.h>
#include <common/io/io_context.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <unordered_map>

BEGIN_NAMESPACE_COMMON
namespace log {

Logger::Logger(std::string name, io::IoContext& io)
    : name_(std::move(name))
    , io_(io)
    , level_(Level::Info)
{
}

void Logger::add_sink(std::shared_ptr<Sink> sink) {
    std::lock_guard<std::mutex> lock(mutex_);
    sinks_.push_back(std::move(sink));
}

void Logger::remove_sink(std::shared_ptr<Sink> sink) {
    std::lock_guard<std::mutex> lock(mutex_);
    sinks_.erase(
        std::remove(sinks_.begin(), sinks_.end(), sink),
        sinks_.end()
    );
}

void Logger::clear_sinks() {
    std::lock_guard<std::mutex> lock(mutex_);
    sinks_.clear();
}

void Logger::set_level(Level level) {
    std::lock_guard<std::mutex> lock(mutex_);
    level_ = level;
}

Level Logger::level() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return level_;
}

void Logger::log(Level level, const std::string& msg) {
    async_log(level, msg);
}

void Logger::async_log(Level level, const std::string& msg, const std::map<std::string, std::string>& fields) {
    if (static_cast<uint8_t>(level) < static_cast<uint8_t>(level_)) {
        return;
    }

    // 异步提交日志任务到 io_context
    io_.post([this, level, msg, fields]() {
        LogMessage log_msg;
        log_msg.level = level;
        log_msg.logger_name = name_;
        log_msg.message = msg;
        log_msg.timestamp = get_timestamp();
        log_msg.thread_id = std::this_thread::get_id();
        log_msg.fields = fields;

        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& sink : sinks_) {
            sink->log(log_msg);
        }
    });
}

void Logger::trace(const std::string& msg) {
    log(Level::Trace, msg);
}

void Logger::debug(const std::string& msg) {
    log(Level::Debug, msg);
}

void Logger::info(const std::string& msg) {
    log(Level::Info, msg);
}

void Logger::warn(const std::string& msg) {
    log(Level::Warn, msg);
}

void Logger::error(const std::string& msg) {
    log(Level::Error, msg);
}

void Logger::critical(const std::string& msg) {
    log(Level::Critical, msg);
}

const std::string& Logger::name() const {
    return name_;
}

io::IoContext& Logger::io_context() {
    return io_;
}

std::string Logger::get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm tm = *std::localtime(&time);

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(4) << (tm.tm_year + 1900) << "-"
        << std::setw(2) << (tm.tm_mon + 1) << "-"
        << std::setw(2) << tm.tm_mday << " "
        << std::setw(2) << tm.tm_hour << ":"
        << std::setw(2) << tm.tm_min << ":"
        << std::setw(2) << tm.tm_sec << "."
        << std::setw(3) << ms.count();
    return oss.str();
}

// 全局 Logger 管理
namespace {
    io::IoContext* g_default_io_context = nullptr;
    std::shared_ptr<Logger> g_default_logger;
    std::unordered_map<std::string, std::shared_ptr<Logger>> g_loggers;
    std::mutex g_loggers_mutex;
}

void init_default_io_context(io::IoContext& io) {
    std::lock_guard<std::mutex> lock(g_loggers_mutex);
    g_default_io_context = &io;
}

Logger& default_logger() {
    std::lock_guard<std::mutex> lock(g_loggers_mutex);
    if (!g_default_logger) {
        if (!g_default_io_context) {
            // 如果没有设置默认 io_context，抛出异常
            throw std::runtime_error("Default io_context not set. Call init_default_io_context() first.");
        }
        g_default_logger = std::make_shared<Logger>("default", *g_default_io_context);
        // 默认添加控制台 sink
        g_default_logger->add_sink(std::make_shared<ConsoleSink>());
    }
    return *g_default_logger;
}

Logger& get_logger(const std::string& name) {
    std::lock_guard<std::mutex> lock(g_loggers_mutex);
    auto it = g_loggers.find(name);
    if (it != g_loggers.end()) {
        return *it->second;
    }
    if (!g_default_io_context) {
        throw std::runtime_error("Default io_context not set. Call init_default_io_context() first.");
    }
    auto logger = std::make_shared<Logger>(name, *g_default_io_context);
    g_loggers[name] = logger;
    return *logger;
}

void set_default_logger(std::shared_ptr<Logger> logger) {
    std::lock_guard<std::mutex> lock(g_loggers_mutex);
    g_default_logger = std::move(logger);
}

} // namespace log
END_NAMESPACE_COMMON
