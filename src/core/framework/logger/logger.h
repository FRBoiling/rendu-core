/*
* Created by boil on 2022/10/16.
*/

#ifndef RENDU_LOGGER_H_
#define RENDU_LOGGER_H_

#include "base_logger.h"

namespace rendu {
  class Logger : public BaseLogger {
  public:
    Logger() : BaseLogger(){};
  public:
    void init(const std::string &flag, RunModeType mode, const std::string &path);
  private:
    RunModeType run_mode_{RunModeType::Develop};
    std::string pattern_str_;
  };
}//namespace rendu



#endif //RENDU_LOGGER_H_
