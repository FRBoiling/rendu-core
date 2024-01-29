/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_TIME_ZONE_H
#define RENDU_TIME_ZONE_H

#include "utils/copyable.h"
#include "date_time.h"

RD_NAMESPACE_BEGIN

// TimeZone for 1970~2100
  class TimeZone : public Copyable {
  public:
    TimeZone() = default;  // an invalid timezone
    TimeZone(int eastOfUtc, const char *tzname);  // a fixed timezone

    static TimeZone UTC();

    static TimeZone China();  // Fixed at GMT+8, no DST
    static TimeZone LoadZoneFile(const char *zonefile);

    // default copy ctor/assignment/dtor are Okay.

    bool valid() const {
      // 'explicit operator bool() const' in C++11
      return static_cast<bool>(data_);
    }

    struct DateTime toLocalTime(int64_t secondsSinceEpoch, int *utcOffset = nullptr) const;

    int64_t fromLocalTime(const struct DateTime &, bool postTransition = false) const;

    // gmtime(3)
    static struct DateTime toUtcTime(int64_t secondsSinceEpoch);

    // timegm(3)
    static int64_t fromUtcTime(const struct DateTime &);

    struct Data;

  private:
    explicit TimeZone(std::unique_ptr<Data> data);

    std::shared_ptr<Data> data_;

    friend class TimeZoneTestPeer;
  };

RD_NAMESPACE_END

#endif //RENDU_TIME_ZONE_H
