/*
* Created by boil on 2022/9/9.
*/

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class LogConsole {
public:
  LogConsole() {
    this->init();
  }

  ~LogConsole() {
    spdlog::drop_all(); // 释放所有logger
  }

  void init() {
    this->init_logger();
  }

  auto get_sink(){
    return console_sink_;
  }
private:

  void init_logger() {
    this->console_sink_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    this->console_sink_->set_level(spdlog::level::trace);
    this->console_sink_->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%t][%l][%L][%^%L%$][%s:%#][%@]%v");
//    spdlog::set_pattern("%+"); // back to default format
  }

private:
  std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink_; // console

}; // LogConsole

