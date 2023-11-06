/*
* Created by boil on 2023/9/24.
*/

#ifndef RENDU_COMMON_TIME_INFO_H
#define RENDU_COMMON_TIME_INFO_H

#include "common/define.h"
#include "date_time.h"

COMMON_NAMESPACE_BEGIN

class TimeInfo {
public:
  DateTime m_curFrameTime;
  DateTime m_lastTime;
};

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_TIME_INFO_H
