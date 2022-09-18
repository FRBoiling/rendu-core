/*
* Created by boil on 2022/9/9.
*/

#include "log_console.h"
#include "log_file.h"

class Log {
public:
  static Log &instance() {
    static Log m_instance;
    return m_instance;
  }

  auto get_logger() const {
    return this->logger_;
  }

private:
  Log() {
    this->init();
  }

  ~Log() {
    spdlog::drop_all(); // 释放所有logger
  }

private:
  void init() {
    _log_console.init();
    _log_file.init();
    init_logger();
  }

  void init_logger() {
    this->sinks_.push_back(_log_console.get_sink());
    this->sinks_.push_back(_log_file.get_sink());

    this->logger_ = std::make_shared<spdlog::logger>("custom", begin(this->sinks_), end(this->sinks_));
    this->logger_->set_level(spdlog::level::trace);
  }

private:
  std::shared_ptr<spdlog::logger> logger_;
  std::vector<spdlog::sink_ptr> sinks_;
  LogFile _log_file;
  LogConsole _log_console;
}; // Log

#define sLog Log::instance().get_logger()