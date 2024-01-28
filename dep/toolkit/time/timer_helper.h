/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_TIMER_HELPER_H
#define RENDU_TIMER_HELPER_H

#include "define.h"
#include "time/local_time.h"
#include "utils/once_token.h"

RD_NAMESPACE_BEGIN

/**
 * 获取时间差, 返回值单位为秒
 */
  long GetGMTOff();

/**
 * 获取1970年至今的毫秒数
 * @param system_time 是否为系统时间(系统时间可以回退),否则为程序启动时间(不可回退)
 */
  uint64_t GetCurrentMillisecond(bool system_time = false);

/**
 * 获取1970年至今的微秒数
 * @param system_time 是否为系统时间(系统时间可以回退),否则为程序启动时间(不可回退)
 */
  uint64_t GetCurrentMicrosecond(bool system_time = false);

/**
 * 获取时间字符串
 * @param fmt 时间格式，譬如%Y-%m-%d %H:%M:%S
 * @return 时间字符串
 */
  std::string GetTimeStr(const char *fmt, time_t time = 0);

/**
 * 根据unix时间戳获取本地时间
 * @param sec unix时间戳
 * @return tm结构体
 */
  struct tm GetLocalTime(time_t sec);


RD_NAMESPACE_END

#endif //RENDU_TIMER_HELPER_H
