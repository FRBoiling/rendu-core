/*
* Created by boil on 2022/10/16.
*/

#include "logger.h"

void rendu::Logger::init(const std::string &flag, RunModeType mode, const std::string &path) {
  run_mode_ = mode;
  spdlog::level::level_enum level;
  switch (run_mode_) {
    case RunModeType::Develop:
      BaseLogger::init(flag, level, path, true, pattern_str_);
      level = spdlog::level::trace;
      break;
    case RunModeType::Online:
    case RunModeType::Pressure:
      level = spdlog::level::info;
      BaseLogger::init(flag, level, path, false, pattern_str_);
      break;
  }
}
