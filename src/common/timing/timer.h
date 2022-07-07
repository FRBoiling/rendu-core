// This file is part of the rendu-core Project. See AUTHORS file for Copyright information
// Created by Boil on 2022/7/6.
//

#ifndef RENDU_SRC_COMMON_TIMING_TIMER_H_
#define RENDU_SRC_COMMON_TIMING_TIMER_H_

#include "define.h"
#include "duration.h"

inline TimePoint GetApplicationStartTime()
{
  using namespace std::chrono;

  static const steady_clock::time_point ApplicationStartTime = steady_clock::now();

  return ApplicationStartTime;
}

inline uint32 getMSTime()
{
  using namespace std::chrono;

  return uint32(duration_cast<milliseconds>(steady_clock::now() - GetApplicationStartTime()).count());
}

inline uint32 getMSTimeDiff(uint32 oldMSTime, uint32 newMSTime)
{
  // getMSTime() have limited data range and this is case when it overflow in this tick
  if (oldMSTime > newMSTime)
    return (0xFFFFFFFF - oldMSTime) + newMSTime;
  else
    return newMSTime - oldMSTime;
}

inline uint32 getMSTimeDiff(uint32 oldMSTime, TimePoint newTime)
{
  using namespace std::chrono;

  uint32 newMSTime = uint32(duration_cast<milliseconds>(newTime - GetApplicationStartTime()).count());
  return getMSTimeDiff(oldMSTime, newMSTime);
}

inline uint32 GetMSTimeDiffToNow(uint32 oldMSTime)
{
  return getMSTimeDiff(oldMSTime, getMSTime());
}

struct IntervalTimer
{
 public:

  IntervalTimer()
      : _interval(0), _current(0)
  {
  }

  void Update(time_t diff)
  {
    _current += diff;
    if (_current < 0)
      _current = 0;
  }

  bool Passed()
  {
    return _current >= _interval;
  }

  void Reset()
  {
    if (_current >= _interval)
      _current %= _interval;
  }

  void SetCurrent(time_t current)
  {
    _current = current;
  }

  void SetInterval(time_t interval)
  {
    _interval = interval;
  }

  time_t GetInterval() const
  {
    return _interval;
  }

  time_t GetCurrent() const
  {
    return _current;
  }

 private:

  time_t _interval;
  time_t _current;
};

struct TimeTracker
{
 public:
  TimeTracker(int32 expiry = 0) : _expiryTime(expiry) { }
  TimeTracker(Milliseconds expiry) : _expiryTime(expiry) { }

  void Update(int32 diff)
  {
    Update(Milliseconds(diff));
  }

  void Update(Milliseconds diff)
  {
    _expiryTime -= diff;
  }

  bool Passed() const
  {
    return _expiryTime <= 0s;
  }

  void Reset(int32 expiry)
  {
    Reset(Milliseconds(expiry));
  }

  void Reset(Milliseconds expiry)
  {
    _expiryTime = expiry;
  }

  Milliseconds GetExpiry() const
  {
    return _expiryTime;
  }

 private:
  Milliseconds _expiryTime;
};

struct PeriodicTimer
{
 public:

  PeriodicTimer(int32 period, int32 start_time)
      : i_period(period), i_expireTime(start_time)
  {
  }

  bool Update(const uint32 diff)
  {
    if ((i_expireTime -= diff) > 0)
      return false;

    i_expireTime += i_period > int32(diff) ? i_period : diff;
    return true;
  }

  void SetPeriodic(int32 period, int32 start_time)
  {
    i_expireTime = start_time;
    i_period = period;
  }

  // Tracker interface
  void TUpdate(int32 diff) { i_expireTime -= diff; }
  bool TPassed() const { return i_expireTime <= 0; }
  void TReset(int32 diff, int32 period)  { i_expireTime += period > diff ? period : diff; }

 private:

  int32 i_period;
  int32 i_expireTime;
};

#endif//RENDU_SRC_COMMON_TIMING_TIMER_H_
