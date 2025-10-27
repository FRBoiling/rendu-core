#include "common/logging/logger.h"
#include "common/logging/default_logger.h"

#include "common/utils/string_format.h"
#include <vector>

BEGIN_NAMESPACE_COMMON
    // Logger类实现
    Logger::Logger()
    {
    }

    Logger::~Logger()
    {
        shutdown();
    }

    Logger& Logger::get_instance()
    {
        static Logger instance;
        return instance;
    }

    bool Logger::initialize(const std::string& name)
    {
        if (!logger_)
            initialize(name, std::make_unique<DefaultLogger>());
        return logger_->initialize(name);
    }

    bool Logger::initialize(const std::string& name, std::unique_ptr<ILogger> logger)
    {
        if (!logger)
            logger_ = std::make_unique<DefaultLogger>();
        else
            logger_ = std::move(logger);
        return logger_->initialize(name);
    }


    void Logger::shutdown()
    {
        logger_->shutdown();
    }

    void Logger::set_level(LogLevel level)
    {
        current_level_ = level;
        logger_->set_level(level);
    }

    // 移除日志输出目标
    bool Logger::remove_sink(const std::string& name)
    {
        return logger_->remove_sink(name);
    }

    // 检查是否包含特定输出目标
    bool Logger::has_sink(const std::string& name) const
    {
        return logger_->has_sink(name);
    }

    // 获取日志输出目标以设置格式等
    std::shared_ptr<LogSink> Logger::get_sink(const std::string& name)
    {
        return  logger_->get_sink(name);;
    }

    // 实现 ILogger 接口的 log 方法
    void Logger::log(LogLevel level, const std::string& message)
    {
        if (!logger_ || level < current_level_) return;

        try
        {
            logger_->log(level, message);
        }
        catch (const std::exception& e)
        {
            // 错误处理
            try
            {
                logger_->log(LogLevel::Error, Utils::StringFormat("日志记录错误: {}", e.what()));
            }
            catch (...)
            {
                // 如果连错误日志都无法记录，则忽略
            }
        }
    }

    // 在Logger类实现部分添加log_impl函数模板的实现
    template <typename... Args>
    void Logger::log_impl(LogLevel level, const char* format, Args&&... args)
    {
        if (!logger_) return;

        try
        {
            // 直接在当前方法中执行格式化，然后调用标准log方法
            auto message = Utils::StringFormat(format, std::forward<Args>(args)...);
            logger_->log(level, message);
        }
        catch (const std::exception& e) // 使用更通用的异常类型，因为StringFormat可能抛出不同的异常
        {
            // 处理格式化错误
            try
            {
                logger_->log(LogLevel::Error, Utils::StringFormat("日志格式化错误: {}", e.what()));
            }
            catch (...)
            {
                // 如果连错误日志都无法记录，则忽略
            }
        }
    }

    // 模板方法实现
    // 由于我们不能在头文件中包含第三方库，所以需要为常用参数类型提供显式实例化
#define INSTANTIATE_LOG_IMPL_NO_ARGS() \
    template void Logger::log_impl<>(LogLevel, const char*);

#define INSTANTIATE_LOG_IMPL_ONE_ARG(T) \
    template void Logger::log_impl<T>(LogLevel, const char*, T&&);

#define INSTANTIATE_LOG_IMPL_TWO_ARGS(T1, T2) \
    template void Logger::log_impl<T1, T2>(LogLevel, const char*, T1&&, T2&&);

#define INSTANTIATE_LOG_IMPL(...) \
    template void Logger::log_impl<__VA_ARGS__>(LogLevel, const char*, __VA_ARGS__&&...);

    // 基本类型的显式实例化
    INSTANTIATE_LOG_IMPL_NO_ARGS() // 无参数
    INSTANTIATE_LOG_IMPL_ONE_ARG(int)
    INSTANTIATE_LOG_IMPL_ONE_ARG(float)
    INSTANTIATE_LOG_IMPL_ONE_ARG(double)
    INSTANTIATE_LOG_IMPL_ONE_ARG(const char*)
    INSTANTIATE_LOG_IMPL_ONE_ARG(std::string)
    INSTANTIATE_LOG_IMPL_TWO_ARGS(int, int)
    INSTANTIATE_LOG_IMPL_TWO_ARGS(const char*, int)
    INSTANTIATE_LOG_IMPL_TWO_ARGS(int, const char*)
    INSTANTIATE_LOG_IMPL_TWO_ARGS(const char*, const char*)


END_NAMESPACE_COMMON