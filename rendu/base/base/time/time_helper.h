/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_BASE_TIME_HELPER_H
#define RENDU_BASE_TIME_HELPER_H

#include "base_define.h"
#include "local_time.h"
#include "number/int_helper.h"
#include "string/string_helper.h"
#include <date/tz.h>
#include <chrono>
#include <iomanip>
#include <utility>

RD_NAMESPACE_BEGIN

  typedef struct tm STime;
  /**
 * 获取时间差, 返回值单位为秒
 */
  std::int64_t GetGMTOff();

  /**
 * 获取1970年至今的毫秒数
 * @param system_time 是否为系统时间(系统时间可以回退),否则为程序启动时间(不可回退)
 */
  std::uint64_t GetCurrentMillisecond(bool system_time = false);

  /**
 * 获取1970年至今的微秒数
 * @param system_time 是否为系统时间(系统时间可以回退),否则为程序启动时间(不可回退)
 */
  std::uint64_t GetCurrentMicrosecond(bool system_time = false);

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
  STime GetLocalTime(time_t sec);

RD_NAMESPACE_END

#endif //RENDU_BASE_TIME_HELPER_H
