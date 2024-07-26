/*
* Created by boil on 2024/1/30.
*/

#include "time_span.h"

RD_TIME_NAMESPACE_BEGIN

TimeSpan::TimeSpan(Long microseconds)
    : m_duration{microseconds} {
}

TimeSpan::TimeSpan(Int days, Int hours, Int minutes, Int seconds, Long milliseconds)
    : m_duration(detail::Days{days} + detail::Hours{hours} + detail::Minutes{minutes} + detail::Seconds{seconds} + detail::Milliseconds{milliseconds}) {
}



RD_TIME_NAMESPACE_END
