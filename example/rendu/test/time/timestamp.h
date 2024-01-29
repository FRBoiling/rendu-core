/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_TIME_TIMESTAMP_H
#define RENDU_TIME_TIMESTAMP_H

#include "time_define.h"

TIME_NAMESPACE_BEGIN

  class Timestamp : public Copyable,
                    public EqualityComparable<Timestamp>,
                    public LessThanComparable<Timestamp> {
  public:
    ///
    /// Constucts an invalid Timestamp.
    ///
    Timestamp()
      : microSecondsSinceEpoch_(0) {
    }

    ///
    /// Constucts a Timestamp at specific time
    ///
    /// @param microSecondsSinceEpoch
    explicit Timestamp(INT64 microSecondsSinceEpochArg)
      : microSecondsSinceEpoch_(microSecondsSinceEpochArg) {
    }

    void swap(Timestamp &that) {
      std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
    }

    // default copy/assignment/dtor are Okay

    STRING toString() const;

    STRING toFormattedString(bool showMicroseconds = true) const;

    bool valid() const { return microSecondsSinceEpoch_ > 0; }

    // for internal usage.
    INT64 microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }

    time_t secondsSinceEpoch() const { return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond); }

    ///
    /// Get time of now.
    ///
    static Timestamp now();

    static Timestamp invalid() {
      return Timestamp();
    }

    static Timestamp fromUnixTime(time_t t) {
      return fromUnixTime(t, 0);
    }

    static Timestamp fromUnixTime(time_t t, int microseconds) {
      return Timestamp(static_cast<INT64>(t) * kMicroSecondsPerSecond + microseconds);
    }

    static const int kMicroSecondsPerSecond = 1000 * 1000;

  private:
    INT64 microSecondsSinceEpoch_;
  };


  inline bool operator<(Timestamp lhs, Timestamp rhs) {
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
  }

  inline bool operator==(Timestamp lhs, Timestamp rhs) {
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
  }

///
/// Gets time difference of two timestamps, result in seconds.
///
/// @param high, low
/// @return (high-low) in seconds
/// @c double has 52-bit precision, enough for one-microsecond
/// resolution for next 100 years.
  inline double timeDifference(Timestamp high, Timestamp low) {
    INT64 diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
  }

///
/// Add @c seconds to given timestamp.
///
/// @return timestamp+seconds as Timestamp
///
  inline Timestamp addTime(Timestamp timestamp, double seconds) {
    INT64 delta = static_cast<INT64>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
  }

TIME_NAMESPACE_END

#endif //RENDU_TIME_TIMESTAMP_H
