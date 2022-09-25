/*
* Created by boil on 2022/9/9.
*/

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // support for user defined types
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <cstdio>
#include <chrono>
#include <spdlog/pattern_formatter.h>
#include "spdlog/stopwatch.h"

class Log {
public:
  static Log &instance() {
    static Log m_instance;
    return m_instance;
  }

  [[nodiscard]] auto get_logger() const {
    return this->logger_;
  }

  void init(const std::string &flag, int mode, const std::string &path) {
    init(flag, spdlog::level::trace, path);
  }

  void init(const std::string &flag, spdlog::level::level_enum level, const std::string &log_root_path) {
    this->init_console();
    std::string log_file_path = flag + ".log";
    this->init_file(log_root_path, log_file_path);
    this->sinks_.push_back(this->console_sink_);
    this->sinks_.push_back(this->file_sink_);
    this->logger_ = std::make_shared<spdlog::logger>(flag, begin(this->sinks_), end(this->sinks_));
    this->logger_->set_level(level);
    //    this->console_sink_->set_pattern("%+");
    this->logger_->flush_on(spdlog::level::trace); // 设置立刻刷新日志到 disk
    spdlog::flush_every(std::chrono::seconds(10)); // 每隔10秒刷新一次日志
    spdlog::register_logger(this->logger_); // 注册logger
  }

private:
  Log() = default;

  ~Log() {
  }

private:
  void init_console() {
    this->console_sink_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    this->console_sink_->set_pattern("[%m-%d %H:%M:%S.%e][%n][%^%L%$] [%!,%s:%#] %v");
  }

  void init_file(const std::string &log_root_path, const std::string &log_file_path) {

    int rotation_h = 5; // 分割时间
    int rotation_m = 59;
    this->file_sink_ = std::make_shared<spdlog::sinks::daily_file_sink_mt>(log_root_path + log_file_path,
                                                                           rotation_h,
                                                                           rotation_m);
    this->file_sink_->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%n][%l][thread:%t][%!,%s:%#] %v");
  }

private:
  std::shared_ptr<spdlog::logger> logger_;
  std::vector<spdlog::sink_ptr> sinks_;
  std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink_; // console
  std::shared_ptr<spdlog::sinks::daily_file_sink_mt> file_sink_; // file

}; // Log

#define RD_INIT(flag, level, path) Log::instance().init(flag,level,path)

#define RD_STOPWATCH spdlog::stopwatch

#define RD_LOGGER_TRACE(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::trace, __VA_ARGS__)
#define RD_TRACE(...) RD_LOGGER_TRACE(Log::instance().get_logger(), __VA_ARGS__)

#define RD_LOGGER_DEBUG(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::debug, __VA_ARGS__)
#define RD_DEBUG(...) RD_LOGGER_DEBUG(Log::instance().get_logger(), __VA_ARGS__)

#define RD_LOGGER_INFO(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::info, __VA_ARGS__)
#define RD_INFO(...) RD_LOGGER_INFO(Log::instance().get_logger(), __VA_ARGS__)

#define RD_LOGGER_WARN(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::warn, __VA_ARGS__)
#define RD_WARN(...) RD_LOGGER_WARN(Log::instance().get_logger(), __VA_ARGS__)

#define RD_LOGGER_ERROR(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::err, __VA_ARGS__)
#define RD_ERROR(...) RD_LOGGER_ERROR(Log::instance().get_logger(), __VA_ARGS__)

#define RD_LOGGER_CRITICAL(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::critical, __VA_ARGS__)
#define RD_CRITICAL(...) RD_LOGGER_CRITICAL(Log::instance().get_logger(), __VA_ARGS__)

