/*
* Created by boil on 2023/12/23.
*/

#ifndef RENDU_LOG_MSG_TIME_H
#define RENDU_LOG_MSG_TIME_H

#include "common_define.h"

COMMON_NAMESPACE_BEGIN

class LogMsgTime {

public:
  LogMsgTime(){
    gettimeofday(&tv_, nullptr);
  }

public:
  [[nodiscard]] std::string ToString() const {
    auto tm = GetLocalTime(tv_.tv_sec);
    char buf[128];
    snprintf(buf, sizeof(buf), "%d-%02d-%02d %02d:%02d:%02d.%03d",
             1900 + tm.tm_year,
             1 + tm.tm_mon,
             tm.tm_mday,
             tm.tm_hour,
             tm.tm_min,
             tm.tm_sec,
             (int) (tv_.tv_usec / 1000));
    return buf;
  }

private:
  timeval tv_{};
};

COMMON_NAMESPACE_END


#endif//RENDU_LOG_MSG_TIME_H
