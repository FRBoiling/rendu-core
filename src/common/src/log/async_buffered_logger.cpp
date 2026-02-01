#include "common/log/async_buffered_logger.h"
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

BEGIN_NAMESPACE_COMMON
    namespace log
    {
        AsyncBufferedLogger::AsyncBufferedLogger(std::string name, io::IoContext& io,
                                                 const AsyncLoggerConfig& config)
            : name_(std::move(name))
              , io_(io)
              , level_(Level::Info)
              , config_(config)
        {
            // 预分配写入缓冲区
            write_buffer_.reserve(config_.buffer_size);
            flush_buffer_.reserve(config_.buffer_size);

            // 预分配消息池
            for (size_t i = 0; i < config_.message_pool_size; ++i)
            {
                message_pool_.push(new LogItem());
            }

            // 启动刷新线程
            flush_thread_ = std::thread(&AsyncBufferedLogger::flush_loop, this);
        }

        AsyncBufferedLogger::~AsyncBufferedLogger()
        {
            // 停止刷新线程
            running_.store(false);
            flush_cv_.notify_all();

            if (flush_thread_.joinable())
            {
                flush_thread_.join();
            }

            // 刷新剩余消息
            flush();

            // 清理消息池
            while (!message_pool_.empty())
            {
                delete message_pool_.front();
                message_pool_.pop();
            }
        }

        void AsyncBufferedLogger::add_sink(std::shared_ptr<Sink> sink)
        {
            std::lock_guard<std::mutex> lock(buffer_mutex_);
            sinks_.push_back(std::move(sink));
        }

        void AsyncBufferedLogger::remove_sink(std::shared_ptr<Sink> sink)
        {
            std::lock_guard<std::mutex> lock(buffer_mutex_);
            sinks_.erase(
                std::remove(sinks_.begin(), sinks_.end(), sink),
                sinks_.end()
            );
        }

        void AsyncBufferedLogger::clear_sinks()
        {
            std::lock_guard<std::mutex> lock(buffer_mutex_);
            sinks_.clear();
        }

        void AsyncBufferedLogger::set_level(Level level)
        {
            level_ = level;
        }

        Level AsyncBufferedLogger::level() const
        {
            return level_;
        }

        void AsyncBufferedLogger::log(Level level, const std::string& message,
                                      const std::map<std::string, std::string>& fields)
        {
            // 检查日志级别
            if (static_cast<uint8_t>(level) < static_cast<uint8_t>(level_))
            {
                return;
            }

            // 从消息池分配消息
            LogItem* item = allocate_log_item();
            if (!item)
            {
                // 消息池耗尽，直接返回（或可以临时分配）
                std::lock_guard<std::mutex> lock(stats_mutex_);
                ++stats_.dropped_count;
                return;
            }

            // 填充消息
            item->level = level;
            item->logger_name = name_;
            item->message = message;
            item->timestamp = get_timestamp();
            item->thread_id = std::this_thread::get_id();
            item->fields = fields;

            // 添加到写入缓冲区
            bool need_flush = false;

            {
                std::lock_guard<std::mutex> lock(buffer_mutex_);
                size_t msg_size = item->message.size() + item->timestamp.size() + item->logger_name.size() + 50; // 估算总大小
                write_buffer_.push_back(std::move(*item));
                write_buffer_bytes_ += msg_size;

                // 更新统计
                {
                    std::lock_guard<std::mutex> stats_lock(stats_mutex_);
                    ++stats_.buffered_count;
                    stats_.buffer_usage_bytes = write_buffer_bytes_;
                }

                // 归还消息项到池
                deallocate_log_item(item);

                // 检查是否需要立即刷新（缓冲区满时总是刷新）
                if (write_buffer_bytes_ >= config_.buffer_size)
                {
                    need_flush = true;
                }
            }

            // 通知刷新线程
            if (need_flush)
            {
                std::lock_guard<std::mutex> lock(flush_mutex_);
                flush_requested_ = true;
            }
            flush_cv_.notify_one();
        }

        void AsyncBufferedLogger::trace(const std::string& msg)
        {
            log(Level::Trace, msg);
        }

        void AsyncBufferedLogger::debug(const std::string& msg)
        {
            log(Level::Debug, msg);
        }

        void AsyncBufferedLogger::info(const std::string& msg)
        {
            log(Level::Info, msg);
        }

        void AsyncBufferedLogger::warn(const std::string& msg)
        {
            log(Level::Warn, msg);
        }

        void AsyncBufferedLogger::error(const std::string& msg)
        {
            log(Level::Error, msg);
        }

        void AsyncBufferedLogger::critical(const std::string& msg)
        {
            log(Level::Critical, msg);
        }

        const std::string& AsyncBufferedLogger::name() const
        {
            return name_;
        }

        void AsyncBufferedLogger::flush()
        {
            size_t flush_count = 0;

            {
                std::lock_guard<std::mutex> lock(buffer_mutex_);

                // 交换缓冲区
                write_buffer_.swap(flush_buffer_);
                write_buffer_bytes_ = 0;
                write_buffer_.clear();
            }

            // 释放锁后再写入 sinks
            if (!flush_buffer_.empty())
            {
                write_to_sinks(flush_buffer_);
                flush_count = flush_buffer_.size();
                flush_buffer_.clear();
            }

            // 更新统计
            {
                std::lock_guard<std::mutex> stats_lock(stats_mutex_);
                stats_.flushed_count += flush_count;
                stats_.buffered_count = 0;
                stats_.buffer_usage_bytes = 0;
            }
        }

        const AsyncLoggerConfig& AsyncBufferedLogger::config() const
        {
            return config_;
        }

        AsyncBufferedLogger::Stats AsyncBufferedLogger::get_stats() const
        {
            std::lock_guard<std::mutex> lock(stats_mutex_);
            return stats_;
        }

        void AsyncBufferedLogger::flush_loop()
        {
            while (running_.load())
            {
                std::unique_lock<std::mutex> lock(flush_mutex_);

                // 等待超时或通知
                if (flush_cv_.wait_for(lock, config_.flush_interval,
                                       [this]() { return !running_.load() || flush_requested_; }))
                {
                    // 被通知（停止或需要刷新）
                    if (flush_requested_)
                    {
                        lock.unlock();  // 在调用 flush() 前释放锁，避免死锁
                        flush();
                        lock.lock();
                        flush_requested_ = false;
                        continue;  // 继续循环
                    }
                    else
                    {
                        // 停止
                        break;
                    }
                }

                // 超时后，检查是否需要自动刷新
                if (config_.auto_flush)
                {
                    // 检查是否有数据需要刷新
                    {
                        std::lock_guard<std::mutex> buffer_lock(buffer_mutex_);
                        if (write_buffer_.empty() && write_buffer_bytes_ == 0)
                        {
                            continue;
                        }
                    }
                    lock.unlock();  // 在调用 flush() 前释放锁，避免死锁
                    flush();
                    lock.lock();
                }
            }
        }

        void AsyncBufferedLogger::write_to_sinks(const std::vector<LogItem>& items)
        {
            // 先获取 sinks 的副本，避免在写入时持有 buffer_mutex_
            std::vector<std::shared_ptr<Sink>> sinks_copy;
            {
                std::lock_guard<std::mutex> lock(buffer_mutex_);
                sinks_copy = sinks_;
            }

            for (const auto& item : items)
            {
                LogMessage log_msg;
                log_msg.level = item.level;
                log_msg.logger_name = item.logger_name;
                log_msg.message = item.message;
                log_msg.timestamp = item.timestamp;
                log_msg.thread_id = item.thread_id;
                log_msg.fields = item.fields;

                for (auto& sink : sinks_copy)
                {
                    sink->log(log_msg);
                }
            }
        }

        std::string AsyncBufferedLogger::get_timestamp()
        {
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

        LogItem* AsyncBufferedLogger::allocate_log_item()
        {
            std::lock_guard<std::mutex> lock(pool_mutex_);

            if (message_pool_.empty())
            {
                return nullptr;
            }

            auto* item = message_pool_.front();
            message_pool_.pop();
            return item;
        }

        void AsyncBufferedLogger::deallocate_log_item(LogItem* item)
        {
            if (!item)
            {
                return;
            }

            std::lock_guard<std::mutex> lock(pool_mutex_);

            // 清空消息内容（可选）
            item->message.clear();
            item->fields.clear();

            message_pool_.push(item);
        }

        // 全局异步缓冲日志器管理
        namespace
        {
            io::IoContext* g_default_async_io_context = nullptr;
            std::shared_ptr<AsyncBufferedLogger> g_default_async_logger;
            std::mutex g_async_logger_mutex;
        }

        void init_default_async_logger(io::IoContext& io,
                                       const AsyncLoggerConfig& config)
        {
            std::lock_guard<std::mutex> lock(g_async_logger_mutex);
            g_default_async_io_context = &io;
            g_default_async_logger = std::make_shared<AsyncBufferedLogger>("default", io, config);
            g_default_async_logger->add_sink(std::make_shared<ConsoleSink>());
        }

        void reset_default_async_logger()
        {
            std::lock_guard<std::mutex> lock(g_async_logger_mutex);
            g_default_async_logger.reset();
            g_default_async_io_context = nullptr;
        }

        AsyncBufferedLogger& default_async_logger()
        {
            std::lock_guard<std::mutex> lock(g_async_logger_mutex);
            if (!g_default_async_logger)
            {
                if (!g_default_async_io_context)
                {
                    throw std::runtime_error("Default io_context not set. Call init_default_async_logger() first.");
                }
                g_default_async_logger = std::make_shared<AsyncBufferedLogger>("default",
                                                                               *g_default_async_io_context);
                g_default_async_logger->add_sink(std::make_shared<ConsoleSink>());
            }
            return *g_default_async_logger;
        }
    } // namespace log
END_NAMESPACE_COMMON
