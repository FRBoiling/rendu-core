#pragma once

#include "common/define.h"
#include "common/log/sink.h"
#include "common/log/console_sink.h"
#include "common/io/io_context.h"
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <queue>
#include <array>

BEGIN_NAMESPACE_COMMON
namespace log {

/**
 * @brief 异步日志配置
 */
struct AsyncLoggerConfig {
    size_t buffer_size = 64 * 1024;  // 单个缓冲区大小 64KB
    size_t buffer_count = 2;           // 双缓冲
    std::chrono::milliseconds flush_interval{100};  // 刷新间隔 100ms
    bool auto_flush = true;             // 自动刷新

    // 预分配日志消息池大小
    size_t message_pool_size = 1000;    // 预分配 1000 个日志消息对象
};

/**
 * @brief 日志消息项（用于缓冲区）
 */
struct LogItem {
    Level level;
    std::string logger_name;
    std::string message;
    std::string timestamp;
    std::thread::id thread_id;
    std::map<std::string, std::string> fields;
};

/**
 * @brief 异步缓冲日志记录器
 *
 * 优化策略：
 * - 批量写入：缓冲区满或超时才 flush
 * - 双缓冲：读写分离，减少锁竞争
 * - 预分配：避免动态内存分配
 */
class AsyncBufferedLogger {
public:
    explicit AsyncBufferedLogger(std::string name, io::IoContext& io,
                               const AsyncLoggerConfig& config = AsyncLoggerConfig{});
    ~AsyncBufferedLogger();

    // 禁止拷贝和移动
    AsyncBufferedLogger(const AsyncBufferedLogger&) = delete;
    AsyncBufferedLogger& operator=(const AsyncBufferedLogger&) = delete;
    AsyncBufferedLogger(AsyncBufferedLogger&&) = delete;
    AsyncBufferedLogger& operator=(AsyncBufferedLogger&&) = delete;

    /**
     * @brief 添加 Sink
     */
    void add_sink(std::shared_ptr<Sink> sink);
    void remove_sink(std::shared_ptr<Sink> sink);
    void clear_sinks();

    /**
     * @brief 设置日志级别
     */
    void set_level(Level level);
    Level level() const;

    /**
     * @brief 记录日志
     * @param level 日志级别
     * @param message 日志消息
     * @param fields 结构化字段
     */
    void log(Level level, const std::string& message,
            const std::map<std::string, std::string>& fields = {});

    // 便捷方法
    void trace(const std::string& msg);
    void debug(const std::string& msg);
    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);
    void critical(const std::string& msg);

    /**
     * @brief 手动刷新缓冲区
     */
    void flush();

    /**
     * @brief 获取配置
     */
    const AsyncLoggerConfig& config() const;

    /**
     * @brief 获取日志器名称
     */
    const std::string& name() const;

    /**
     * @brief 获取统计信息
     */
    struct Stats {
        size_t buffered_count = 0;       // 缓冲区中的消息数
        size_t flushed_count = 0;        // 已刷新的消息数
        size_t dropped_count = 0;        // 丢弃的消息数（缓冲区满）
        size_t buffer_usage_bytes = 0;    // 缓冲区使用字节数
    };
    Stats get_stats() const;

private:
    /**
     * @brief 刷新线程主循环
     */
    void flush_loop();

    /**
     * @brief 将消息写入 sinks
     * @param items 要写入的消息列表
     */
    void write_to_sinks(const std::vector<LogItem>& items);

    /**
     * @brief 获取时间戳
     */
    std::string get_timestamp();

    /**
     * @brief 从消息池获取消息项
     */
    LogItem* allocate_log_item();

    /**
     * @brief 归还消息项到池
     */
    void deallocate_log_item(LogItem* item);

    std::string name_;
    io::IoContext& io_;
    Level level_;
    std::vector<std::shared_ptr<Sink>> sinks_;
    AsyncLoggerConfig config_;

    // 双缓冲机制
    std::vector<LogItem> write_buffer_;   // 写入缓冲区（生产者使用）
    std::vector<LogItem> flush_buffer_;   // 刷新缓冲区（消费者使用）
    size_t write_buffer_bytes_{0};         // 写入缓冲区当前字节数
    std::mutex buffer_mutex_;              // 保护缓冲区切换

    // 消息池（预分配）
    std::queue<LogItem*> message_pool_;
    std::mutex pool_mutex_;

    // 刷新线程
    std::thread flush_thread_;
    std::atomic<bool> running_{true};
    std::condition_variable flush_cv_;
    std::mutex flush_mutex_;
    bool flush_requested_{false};

    // 统计信息
    mutable std::mutex stats_mutex_;
    Stats stats_;
};

// 全局默认异步缓冲日志器
AsyncBufferedLogger& default_async_logger();

// 初始化默认 io_context
void init_default_async_logger(io::IoContext& io,
                             const AsyncLoggerConfig& config = AsyncLoggerConfig{});

// 重置默认异步日志器
void reset_default_async_logger();

} // namespace log
END_NAMESPACE_COMMON
