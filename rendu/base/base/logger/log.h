/*
* Created by boil on 2023/9/24.
*/

#ifndef RENDU_BASE_LOG_H
#define RENDU_BASE_LOG_H

#include "log_define.h"
#include "logger/logger.h"

RD_NAMESPACE_BEGIN

    class Log {
    private:
      Log() = default;

    public:
      virtual ~Log() = default;

    public:
      static Log &Instance() {
        static Log instance;
        return instance;
      }

      Log(Log &&) = delete;

      Log(const Log &) = delete;

      void operator=(const Log &) = delete;

    public:
      template<typename ... Args>
      void Init(Args ...args) {
        m_logger_.Init(args...);
      }

      Logger &GetLogger() { return m_logger_; }


    private:
      Logger m_logger_;
    };

#define RD_LOGGER Log::Instance().GetLogger().get_logger()
#define RD_LOGGER_INIT(...) Log::Instance().Init(__VA_ARGS__)
#define RD_STOPWATCH spdlog::stopwatch

//封装宏，没有该宏无法输出文件名、行号等信息
#define RD_TRACE(...) SPDLOG_LOGGER_CALL(RD_LOGGER, spdlog::level::trace, __VA_ARGS__)
#define RD_DEBUG(...) SPDLOG_LOGGER_CALL(RD_LOGGER, spdlog::level::debug, __VA_ARGS__)
#define RD_INFO(...) SPDLOG_LOGGER_CALL(RD_LOGGER, spdlog::level::info, __VA_ARGS__)
#define RD_WARN(...) SPDLOG_LOGGER_CALL(RD_LOGGER, spdlog::level::warn, __VA_ARGS__)
#define RD_ERROR(...) SPDLOG_LOGGER_CALL(RD_LOGGER, spdlog::level::err, __VA_ARGS__)
#define RD_CRITICAL(...) SPDLOG_LOGGER_CALL(RD_LOGGER, spdlog::level::critical, __VA_ARGS__)

RD_NAMESPACE_END

#endif //RENDU_BASE_LOG_H
