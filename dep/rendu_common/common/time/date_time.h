/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_DATE_TIME_H
#define RENDU_DATE_TIME_H

#include "common/define.h"

RD_NAMESPACE_BEGIN
// Local time in unspecified timezone.
// A minute is always 60 seconds, no leap seconds.
  struct DateTime {
    DateTime() {}

    explicit DateTime(const struct tm &);

    DateTime(int _year, int _month, int _day, int _hour, int _minute, int _second)
      : year(_year), month(_month), day(_day), hour(_hour), minute(_minute), second(_second) {
    }

    // "2011-12-31 12:34:56"
    string toIsoString() const;

    int year = 0;     // [1900, 2500]
    int month = 0;    // [1, 12]
    int day = 0;      // [1, 31]
    int hour = 0;     // [0, 23]
    int minute = 0;   // [0, 59]
    int second = 0;   // [0, 59]
  };

RD_NAMESPACE_END
#endif //RENDU_DATE_TIME_H
