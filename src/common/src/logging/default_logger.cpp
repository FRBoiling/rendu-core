//
// Created by 沸腾 on 2025/10/31.
//

#include "common/logging/default_logger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>


BEGIN_NAMESPACE_COMMON

    class DefaultSinkImpl final : public LogSink
    {
    public:
        [[nodiscard]] std::shared_ptr<spdlog::sinks::sink> sink() const
        {
            return sink_;
        }

        DefaultSinkImpl();
        ~DefaultSinkImpl() override;
        void set_pattern(const std::string& pattern) override;
        const std::string& get_name() const override;

    private:
        std::string name_;
        std::shared_ptr<spdlog::sinks::sink> sink_;
    };

    DefaultSinkImpl::DefaultSinkImpl()
    {
        name_ = "default_sink";
        sink_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    }

    DefaultSinkImpl::~DefaultSinkImpl()
    {
    }

    void DefaultSinkImpl::set_pattern(const std::string& pattern)
    {
    }

    const std::string& DefaultSinkImpl::get_name() const
    {
    }


    DefaultSink::DefaultSink() : m_impl(new DefaultSinkImpl())
    {
    }

    DefaultSink::~DefaultSink()
    {
        delete m_impl;
    }

    void DefaultSink::set_pattern(const std::string& pattern)
    {
        m_impl->set_pattern(pattern);
    }

    const std::string& DefaultSink::get_name() const
    {
        return m_impl->get_name();
    }

    DefaultSinkImpl* DefaultSink::get_impl() const
    {
        return m_impl;
    }


    class DefaultLoggerImpl final : public ILogger
    {
    public:
        DefaultLoggerImpl() = default;

        bool initialize(const std::string& name) override
        {
            try
            {
                // 初始化spdlog
                spdlog::set_error_handler([](const std::string& msg) {
                    std::cerr << "[spdlog error] " << msg << std::endl;
                });

                // 创建默认控制台sink
                auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
                
                // 存储默认sink
                sinks_["console"] = console_sink;

                // 创建logger
                logger_ = std::make_shared<spdlog::logger>(name, console_sink);
                logger_->set_level(spdlog::level::debug);
                logger_->flush_on(spdlog::level::critical);

                // 注册为全局logger
                spdlog::register_logger(logger_);

                return true;
            }
            catch (const std::exception& e)
            {
                std::cerr << "Logger initialization failed: " << e.what() << std::endl;
                return false;
            }
        }

        void shutdown() override
        {
            if (logger_)
            {
                logger_->flush();
                spdlog::drop_all();
                logger_.reset();
                sinks_.clear();
                contexts_.clear();
            }
        }

        void set_level(LogLevel level) override
        {
            if (!logger_) return;
            current_level_ = level;
            switch (current_level_)
            {
            case LogLevel::Debug: logger_->set_level(spdlog::level::debug);
                break;
            case LogLevel::Info: logger_->set_level(spdlog::level::info);
                break;
            case LogLevel::Warn: logger_->set_level(spdlog::level::warn);
                break;
            case LogLevel::Error: logger_->set_level(spdlog::level::err);
                break;
            case LogLevel::Critical: logger_->set_level(spdlog::level::critical);
                break;
            case LogLevel::Off: logger_->set_level(spdlog::level::off);
                break;
            default: break;
            }
        }

        void log(LogLevel level, const std::string& message) override
        {
            if (!logger_) return;

            switch (level)
            {
            case LogLevel::Debug: logger_->log(spdlog::level::debug, "{}", message);
                break;
            case LogLevel::Info: logger_->log(spdlog::level::info, "{}", message);
                break;
            case LogLevel::Warn: logger_->log(spdlog::level::warn, "{}", message);
                break;
            case LogLevel::Error: logger_->log(spdlog::level::err, "{}", message);
                break;
            case LogLevel::Critical: logger_->log(spdlog::level::critical, "{}", message);
                break;
            default: break;
            }
        }

        bool add_sink(std::shared_ptr<LogSink> sink) override
        {
            if (!logger_ || !sink)
            {
                return false;
            }

            try
            {
                const std::string& name = sink->get_name();
                
                // 检查sink是否已存在
                if (sinks_.find(name) != sinks_.end()) {
                    return false;
                }
                
                // 使用工厂函数获取spdlog sink
                auto spdlog_sink = get_spdlog_sink(sink.get());
                if (!spdlog_sink) {
                    return false;
                }
                
                sinks_[name] = spdlog_sink;
                rebuild_logger();
                
                // 应用上下文信息到新sink
                apply_contexts_to_sink(spdlog_sink);
                
                return true;
            }
            catch (const std::exception& e)
            {
                std::cerr << "Failed to add sink: " << e.what() << std::endl;
                return false;
            }
        }

        bool remove_sink(const std::string& name) override
        {
            if (!logger_ || sinks_.find(name) == sinks_.end())
            {
                return false;
            }

            // 确保至少保留一个sink
            if (sinks_.size() <= 1)
            {
                return false;
            }

            sinks_.erase(name);
            rebuild_logger();
            return true;
        }

        bool has_sink(const std::string& name) const override
        {
            return sinks_.find(name) != sinks_.end();
        }

        // Add this function somewhere in the file, preferably near the get_spdlog_sink function
        std::shared_ptr<LogSink> create_spdlog_sink(std::shared_ptr<spdlog::sinks::sink>* sink_ptr)
        {
            if (!sink_ptr || !*sink_ptr) {
                return nullptr;
            }
            
            // Create a wrapper class that implements LogSink interface and holds the spdlog sink
            class SpdlogSinkWrapper : public LogSink {
            public:
                explicit SpdlogSinkWrapper(std::shared_ptr<spdlog::sinks::sink> sink)
                    : sink_(std::move(sink)), name_("spdlog_sink") {}
                
                void set_pattern(const std::string& pattern) override {
                    if (sink_) {
                        sink_->set_pattern(pattern);
                    }
                }
                
                std::string get_pattern() const override {
                    // Note: spdlog doesn't provide a direct way to get the current pattern
                    return ""; // Or implement pattern tracking if needed
                }
                
                const std::string& get_name() const override {
                    return name_;
                }
                
                void set_min_level(LogLevel level) override {
                    // Implementation depends on how sink filtering is handled
                }
                
                void flush() override {
                    if (sink_) {
                        sink_->flush();
                    }
                }
                
                std::shared_ptr<spdlog::sinks::sink> get_spdlog_sink() const {
                    return sink_;
                }
                
            private:
                std::shared_ptr<spdlog::sinks::sink> sink_;
                std::string name_;
            };
            
            // Create and return the wrapper
            return std::make_shared<SpdlogSinkWrapper>(*sink_ptr);
        }

        void rebuild_logger()
        {
            if (!logger_) return;

            auto level = logger_->level();
            std::string name = logger_->name();

            std::vector<std::shared_ptr<spdlog::sinks::sink>> sink_list;
            for (const auto& pair : sinks_)
            {
                sink_list.push_back(pair.second->get_impl()->sink());
            }

            logger_ = std::make_shared<spdlog::logger>(name, sink_list.begin(), sink_list.end());
            logger_->set_level(level);
            spdlog::register_logger(logger_);
        }

        void flush() override {
            if (logger_) {
                logger_->flush();
            }
        }
        
        void set_global_pattern(const std::string& pattern) override {
            if (logger_) {
                for (auto& [name, sink] : sinks_) {
                    sink->set_pattern(pattern);
                }
            }
        }
        
        void add_context(const std::string& key, const std::string& value) override {
            contexts_[key] = value;
            
            // 应用到现有sink
            if (logger_) {
                for (auto& [name, sink] : sinks_) {
                    apply_context_to_sink(sink, key, value);
                }
            }
        }
        
        void remove_context(const std::string& key) override {
            auto it = contexts_.find(key);
            if (it != contexts_.end()) {
                contexts_.erase(it);
                
                // 从sink中移除
                if (logger_) {
                    for (auto& [name, sink] : sinks_) {
                        remove_context_from_sink(sink, key);
                    }
                }
            }
        }

        std::shared_ptr<spdlog::logger> logger_;
        std::unordered_map<std::string, std::shared_ptr<spdlog::sinks::sink>> sinks_;
        std::unordered_map<std::string, std::string> contexts_;
        
        // 从LogSink获取spdlog sink的辅助方法
        std::shared_ptr<spdlog::sinks::sink> get_spdlog_sink(LogSink* sink) {
            // 实际实现根据具体LogSink类型获取对应的spdlog sink
            // 这里简化处理
            return nullptr;
        }
        
        // 应用上下文到sink
        void apply_context_to_sink(std::shared_ptr<spdlog::sinks::sink> sink, 
                                   const std::string& key, const std::string& value) {
            // 实现将上下文信息应用到sink
        }
        
        // 从sink移除上下文
        void remove_context_from_sink(std::shared_ptr<spdlog::sinks::sink> sink, 
                                     const std::string& key) {
            // 实现从sink移除上下文信息
        }
        
        // 应用所有上下文到sink
        void apply_contexts_to_sink(std::shared_ptr<spdlog::sinks::sink> sink) {
            for (const auto& [key, value] : contexts_) {
                apply_context_to_sink(sink, key, value);
            }
        }
    };

END_NAMESPACE_COMMON