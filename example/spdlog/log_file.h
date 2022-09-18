/*
* Created by boil on 2022/9/9.
*/

#ifndef RENDU_LOG_FILE_H_
#define RENDU_LOG_FILE_H_
#include <spdlog/spdlog.h>
//#include <spdlog/cfg/env.h>  // support for loading levels from the environment variable
//#include <spdlog/fmt/ostr.h> // support for user defined types
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <cstdio>
#include <chrono>

class LogFile {
public:
  LogFile() {
    this->init();
  }

  ~LogFile() {
    spdlog::drop_all(); // 释放所有logger
  }

  void init() {
    this->init_file();
    this->init_logger();
  }

  auto get_sink(){
    return file_sink_;
  }
private:

  void init_file() {
    this->log_root_path = "logs/";
    this->log_file_path = "custom.log";
    this->rotation_h = 5; // 分割时间
    this->rotation_m = 59;
  }

  void init_logger() {
    this->file_sink_ = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
        this->log_root_path + this->log_file_path, this->rotation_h, this->rotation_m);
    this->file_sink_ ->set_level(spdlog::level::trace);
  }

private:
  std::shared_ptr<spdlog::sinks::daily_file_sink_mt> file_sink_; // file
  std::string log_root_path;
  std::string log_file_path;
  short int rotation_h{};
  short int rotation_m{};

}; // LogFile
#endif //RENDU_LOG_FILE_H_
