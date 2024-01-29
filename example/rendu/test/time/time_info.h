/*
* Created by boil on 2023/9/24.
*/

#ifndef RENDU_TIME_TIME_INFO_H
#define RENDU_TIME_TIME_INFO_H

#include "time_define.h"
#include "date_time.h"

TIME_NAMESPACE_BEGIN

class TimeInfo {
public:
  DateTime m_curFrameTime;
  DateTime m_lastTime;
};

TIME_NAMESPACE_END

#endif //RENDU_TIME_TIME_INFO_H
