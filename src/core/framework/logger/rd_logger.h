/*
* Created by boil on 2022/10/16.
*/

#ifndef RENDU_RD_LOGGER_H_
#define RENDU_RD_LOGGER_H_

#include "a_logger.h"

namespace rendu {
  class RDLogger : public ALogger {
  public:
    RDLogger() :ALogger(){};
  public:
    void init(const std::string &flag, RunModeType mode, const std::string &path);
  private:
    RunModeType run_mode_{RunModeType::Develop};
    std::string pattern_str_;
  };
}//namespace rendu



#endif //RENDU_RD_LOGGER_H_
