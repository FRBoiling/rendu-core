/*
* Created by boil on 2023/12/23.
*/

#ifndef RENDU_LOG_MSG_TIME_H
#define RENDU_LOG_MSG_TIME_H

#include "log_define.h"
#include "time/time.hpp"

LOG_NAMESPACE_BEGIN

class LogMsgTime {

public:
  LogMsgTime(): m_date_time(time::DateTime::Now()){
  }

public:
  [[nodiscard]] std::string ToString() const {
    return m_date_time.ToString();
  }

private:
  time::DateTime m_date_time;
};

LOG_NAMESPACE_END


#endif//RENDU_LOG_MSG_TIME_H
