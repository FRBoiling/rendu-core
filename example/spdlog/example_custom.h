/*
* Created by boil on 2022/9/18.
*/

#ifndef RENDU_EXAMPLE_CUSTOM_H_
#define RENDU_EXAMPLE_CUSTOM_H_

#include <spdlog/spdlog.h>
//#include <spdlog/cfg/env.h>  // support for loading levels from the environment variable
//#include <spdlog/fmt/ostr.h> // support for user defined types
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <cstdio>
#include <chrono>

class CustomLog {

public:
  static CustomLog &instance() {
    static CustomLog m_instance;
    return m_instance;
  }

  auto get_logger() const {
    return this->logger_;
  }

private:
  CustomLog() {
    this->init();
  }

  ~CustomLog() {
    spdlog::drop_all(); // 释放所有logger
  }

private:
  void init() {
    this->init_file();
    this->init_logger();
  }

  void init_file() {
    this->log_root_path = "logs/";
    this->log_file_path = "custom.log";
    this->rotation_h = 5; // 分割时间
    this->rotation_m = 59;
  }

  void init_logger() {

    this->file_sink_ = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
        this->log_root_path + this->log_file_path, this->rotation_h, this->rotation_m);
    this->console_sink_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    this->file_sink_->set_level(spdlog::level::trace);
    this->console_sink_->set_level(spdlog::level::trace);

    this->sinks_.push_back(this->file_sink_); // file
    this->sinks_.push_back(this->console_sink_); // console

    this->logger_ = std::make_shared<spdlog::logger>("log_demo", begin(this->sinks_), end(this->sinks_));
    this->logger_->set_pattern("[%l] [%Y-%m-%d %H:%M:%S,%e] [Process:%P] - %v");
    this->logger_->set_level(spdlog::level::trace);
    //    this->logger_->flush_on(spdlog::level::trace); // 设置立刻刷新日志到 disk
//    spdlog::flush_every(std::chrono::seconds(10)); // 每隔10秒刷新一次日志
//    spdlog::register_logger(this->logger_); // 注册logger
    spdlog::set_default_logger(this->logger_);

  }

private:
  std::shared_ptr<spdlog::logger> logger_;
  std::shared_ptr<spdlog::sinks::daily_file_sink_mt> file_sink_; // file
  std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink_; // console
  std::vector<spdlog::sink_ptr> sinks_;
  std::string log_root_path;
  std::string log_file_path;
  short int rotation_h{};
  short int rotation_m{};

}; // CustomLog


void custom_example() {
#
  CustomLog::instance();
  int i = 0;
  while (true){
    i++;
    SPDLOG_TRACE("message #################{}",i);
    SPDLOG_DEBUG("message #################{}",i);
    SPDLOG_WARN("message #################{}",i);
    SPDLOG_INFO("message #################{}",i);
    SPDLOG_ERROR("message #################{}",i);
  }
}


#endif //RENDU_EXAMPLE_CUSTOM_H_
