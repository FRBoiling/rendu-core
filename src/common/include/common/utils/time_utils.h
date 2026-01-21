//
// Created by 沸腾 on 2025/11/22.
//

#ifndef RENDU_TIME_UTILS_H
#define RENDU_TIME_UTILS_H

#include <string>

#include "common/define.h"

BEGIN_NAMESPACE_COMMON
    namespace Utils
    {
        //enum LocaleConstant : uint8;

        enum class TimeFormat : uint8
        {
            FullText, // 1 Days 2 Hours 3 Minutes 4 Seconds
            ShortText, // 1d 2h 3m 4s
            Numeric // 1:2:3:4
        };


        RC_COMMON_API time_t GetLocalHourTimestamp(time_t time, uint8 hour, bool onlyAfterTime = true);
        RC_COMMON_API tm TimeBreakdown(time_t time);

        RC_COMMON_API std::string secsToTimeString(uint64 timeInSecs, TimeFormat timeFormat = TimeFormat::FullText,
                                                   bool hoursOnly = false);
        RC_COMMON_API uint32 TimeStringToSecs(std::string const& timestring);
        RC_COMMON_API std::string TimeToTimestampStr(time_t t);
        RC_COMMON_API std::string TimeToHumanReadable(time_t t);


    } // namespace Utils


END_NAMESPACE_COMMON


#endif //RENDU_TIME_UTILS_H
