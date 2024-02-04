/*
* Created by boil on 2024/2/4.
*/

#ifndef RENDU_LOG_DEFAULT_LOGGER_H_
#define RENDU_LOG_DEFAULT_LOGGER_H_

#include "log_define.h"

LOG_NAMESPACE_BEGIN

class Logger : public ALogger {
public:
  Logger(std::string flag = "defaultlogger") : ALogger(flag){};
  ~Logger() override = default;

  void InitChannel() override;
};

LOG_NAMESPACE_END

#endif//RENDU_LOG_DEFAULT_LOGGER_H_
