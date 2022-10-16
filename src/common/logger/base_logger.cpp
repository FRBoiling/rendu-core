/*
* Created by boil on 2022/10/16.
*/
#include "base_logger.h"

namespace rendu{
  void BaseLogger::init_console(const string &flag) {
    this->console_sink_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    this->console_sink_->set_pattern(console_pattern_);
    //    this->console_sink_->set_pattern("%+");
    this->sinks_.push_back(this->console_sink_);
  }

  void BaseLogger::init_file(const string &flag, const string &log_root_path) {
    int rotation_h = 5; // 分割时间
    int rotation_m = 59;
    std::string log_file_path = flag + ".log";
    this->file_sink_ = std::make_shared<spdlog::sinks::daily_file_sink_mt>(log_root_path + log_file_path,
                                                                           rotation_h,
                                                                           rotation_m);
    this->file_sink_->set_pattern(file_pattern_);
    this->sinks_.push_back(this->file_sink_);
  }

  void BaseLogger::init(spdlog::level::level_enum level, const string &log_path, bool console,
                        const string &pattern_str) {
    level_ = level;
    if (!pattern_str.empty()) {
      console_pattern_ = pattern_str;
      file_pattern_ = pattern_str;
    }

    if (console) {
      init_console(flag_);
    }
    init_file(flag_, log_path);
    this->logger_ = std::make_shared<spdlog::logger>(flag_, begin(this->sinks_), end(this->sinks_));
    this->logger_->set_level(level_);
    this->logger_->flush_on(level_); // 设置立刻刷新日志到 disk
    spdlog::flush_every(std::chrono::seconds(10)); // 每隔10秒刷新一次日志
    spdlog::register_logger(this->logger_); // 注册logger
  }


}

