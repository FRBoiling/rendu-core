/*
* Created by boil on 2023/11/2.
*/

#include "condition.h"

COMMON_NAMESPACE_BEGIN

  bool Condition::waitForSeconds(double seconds) {
    struct timespec abstime;
    // FIXME: use CLOCK_MONOTONIC or CLOCK_MONOTONIC_RAW to prevent time rewind.
    clock_gettime(CLOCK_REALTIME, &abstime);

    const INT64 kNanoSecondsPerSecond = 1000000000;
    INT64 nanoseconds = static_cast<INT64>(seconds * kNanoSecondsPerSecond);

    abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / kNanoSecondsPerSecond);
    abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) % kNanoSecondsPerSecond);

    MutexLock::UnassignGuard ug(mutex_);
    return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);
  }

COMMON_NAMESPACE_END