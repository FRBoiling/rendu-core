/*
* Created by boil on 2023/9/24.
*/

#ifndef RENDU_TIME_INFO_H
#define RENDU_TIME_INFO_H

#include "common/define.h"
#include "date_time.h"

RD_NAMESPACE_BEGIN

class TimeInfo {
public:
  DateTime m_curFrameTime;
  DateTime m_lastTime;
};

RD_NAMESPACE_END

#endif //RENDU_TIME_INFO_H
