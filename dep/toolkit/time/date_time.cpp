/*
* Created by boil on 2023/11/2.
*/

#include "date_time.h"

RD_NAMESPACE_BEGIN

  DateTime::DateTime(const struct tm &t)
    : year(t.tm_year + 1900), month(t.tm_mon + 1), day(t.tm_mday),
      hour(t.tm_hour), minute(t.tm_min), second(t.tm_sec) {
  }

  string DateTime::toIsoString() const {
    char buf[64];
    snprintf(buf, sizeof buf, "%04d-%02d-%02d %02d:%02d:%02d",
             year, month, day, hour, minute, second);
    return buf;
  }


RD_NAMESPACE_END
