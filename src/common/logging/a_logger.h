/*
* Created by boil on 2022/9/9.
*/
#ifndef RENDU_A_LOGGER_H_
#define RENDU_A_LOGGER_H_

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // support for user defined types
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <cstdio>
#include <chrono>
#include <spdlog/pattern_formatter.h>
#include <spdlog/stopwatch.h>

namespace rendu {

  class ALogger {
  public:
    ALogger() =default;
    explicit ALogger(const std::string &flag) :level_(spdlog::level::info){
      init(flag,level_, "", true,"%+");
    }
    virtual ~ALogger(){
      spdlog::shutdown();
    }
    [[nodiscard]] std::shared_ptr<spdlog::logger> get_logger() const {
      return this->logger_;
    }
  protected:
    void init(const std::string &flag, spdlog::level::level_enum& level, const std::string &log_path, bool console,
              const std::string &pattern_str);

    void init_console(const std::string &flag);

    void init_file(const std::string &flag, const std::string &log_root_path);
  private:
    spdlog::level::level_enum level_;
    std::shared_ptr<spdlog::logger> logger_;
    std::vector<spdlog::sink_ptr> sinks_;
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink_; // console
    std::shared_ptr<spdlog::sinks::daily_file_sink_mt> file_sink_; // file
    std::string console_pattern_{"[%m-%d %H:%M:%S.%e][%n][%^%L%$] [%s:%#] %v"};
    std::string file_pattern_{"[%Y-%m-%d %H:%M:%S.%e][%n][%l][thread:%t][%!,%s:%#] %v"};

  }; // Logger

}//namespace rendu

#endif // RENDU_A_LOGGER_H_
