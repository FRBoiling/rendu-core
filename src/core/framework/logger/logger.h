/*
* Created by boil on 2022/10/16.
*/

#ifndef RENDU_LOGGER_H_
#define RENDU_LOGGER_H_

#include "base_logger.h"

namespace rendu {
  class Logger : public BaseLogger {
  public:
    Logger(const std::string &flag, RunModeType run_mode, const std::string &path)
        : BaseLogger(flag), run_mode_(run_mode) {
      pattern_str_ = "";
      switch (run_mode_) {
        case RunModeType::Develop:
          BaseLogger::init(spdlog::level::trace, path, true, "");
          break;
        case RunModeType::Online:
        case RunModeType::Pressure:
          BaseLogger::init(spdlog::level::info, path, false, "");
          break;
      }
    }

  private:
    std::string pattern_str_;
    RunModeType run_mode_;
  };
}//namespace rendu



#endif //RENDU_LOGGER_H_
