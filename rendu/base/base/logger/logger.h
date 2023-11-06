/*
* Created by boil on 2023/9/24.
*/

#ifndef RENDU_BASE_LOGGER_H
#define RENDU_BASE_LOGGER_H

#include "base/define.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // support for user defined types
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <cstdio>
#include <chrono>
#include <utility>
#include <spdlog/pattern_formatter.h>
#include <spdlog/stopwatch.h>
#include <spdlog/sinks/hourly_file_sink.h>

RD_NAMESPACE_BEGIN

    class Logger {
    public:
      //日志输出位置
      enum LogPosition {
        CONSOLE = 0x01, //控制台
        FILE = 0X02, //文件
        CONSOLE_AND_FILE = 0x03, //控制台+文件
      };

      enum LogMode {
        SYNC, //同步模式
        ASYNC, //异步模式
      };

      //日志输出等级
      enum LogLevel : int {
        LEVEL_TRACE = SPDLOG_LEVEL_TRACE,
        LEVEL_DEBUG = SPDLOG_LEVEL_DEBUG,
        LEVEL_INFO = SPDLOG_LEVEL_INFO,
        LEVEL_WARN = SPDLOG_LEVEL_WARN,
        LEVEL_ERROR = SPDLOG_LEVEL_ERROR,
        LEVEL_CRITICAL = SPDLOG_LEVEL_CRITICAL,
        LEVEL_OFF = SPDLOG_LEVEL_OFF,
      };

    public:
      Logger();

      virtual ~Logger() = default;

    public:
      /* func: 初始化日志通道
       * @para [in] outPath: 日志存储路径   （支持相对路径和绝对路径）
       * @para [in] outFlag: 日志文件标记   （文件名头）
       * @para [in] outMode : 日志输出模式   （同步、异步）
       * @para [in] outMode : 日志输出模式   （同步、异步）
       * @para [in] outPos : 日志输出位置   （控制台、文件、控制台+文件）
       * @para [in] outLevel : 日志输出等级   （只输出>=等级的日志消息）
       */
      bool Init(const std::string &logPath,
                const std::string &logFlag,
                const std::string &pattern_str = "",
                LogLevel logLevel = LogLevel::LEVEL_TRACE,
                LogMode logMode = LogMode::SYNC,
                LogPosition logPos = LogPosition::CONSOLE_AND_FILE
      );

      [[nodiscard]] std::shared_ptr <spdlog::logger> get_logger() const {
        if (flag_.empty()) return spdlog::default_logger();
//        spdlog::get(RD_LOG_NAME)
        return this->logger_;
      }

    private:

      void init_console();

      void init_file(const std::string &log_root_path);

    private:
      spdlog::level::level_enum level_;
      std::string flag_;
      std::string log_root_path_;
    private:
      std::shared_ptr <spdlog::logger> logger_;
      std::vector <spdlog::sink_ptr> sinks_;
      std::shared_ptr <spdlog::sinks::stdout_color_sink_mt> console_sink_; // console
      std::shared_ptr <spdlog::sinks::hourly_file_sink_mt> file_sink_; // file
      std::string console_pattern_{"[%m-%d %H:%M:%S.%e][%n][%^%L%$] [%s:%#] %v"};
      std::string file_pattern_{"[%Y-%m-%d %H:%M:%S.%e][%n][%l][thread:%t][%!,%s:%#] %v"};
    };


RD_NAMESPACE_END

#endif //RENDU_BASE_LOGGER_H


