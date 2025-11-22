//
// Created by 沸腾 on 2025/11/22.
//

#include "common.h"
#include "common/utils/time_utils.h"

#include "common/utils/optional.h"
#include "common/utils/string_convert.h"
#include "common/utils/string_format.h"
#include "common/utils/string_utils.h"

BEGIN_NAMESPACE_COMMON
    namespace Utils
    {
        tm TimeBreakdown(time_t time)
        {
            tm timeLocal;
            localtime_r(&time, &timeLocal);
            return timeLocal;
        }


        time_t GetLocalHourTimestamp(time_t time, uint8 hour, bool onlyAfterTime)
        {
            tm timeLocal = TimeBreakdown(time);
            timeLocal.tm_hour = 0;
            timeLocal.tm_min = 0;
            timeLocal.tm_sec = 0;
            time_t midnightLocal = mktime(&timeLocal);
            time_t hourLocal = midnightLocal + hour * HOUR;

            if (onlyAfterTime && hourLocal <= time)
                hourLocal += DAY;

            return hourLocal;
        }

        std::string secsToTimeString(uint64 timeInSecs, TimeFormat timeFormat, bool hoursOnly)
        {
            uint64 secs = timeInSecs % MINUTE;
            uint64 minutes = timeInSecs % HOUR / MINUTE;
            uint64 hours = timeInSecs % DAY / HOUR;
            uint64 days = timeInSecs / DAY;

            if (timeFormat == TimeFormat::Numeric)
            {
                if (days)
                    return StringFormat("{}:{:02}:{:02}:{:02}", days, hours, minutes, secs);
                else if (hours)
                    return StringFormat("{}:{:02}:{:02}", hours, minutes, secs);
                else if (minutes)
                    return StringFormat("{}:{:02}", minutes, secs);
                else
                    return StringFormat("0:{:02}", secs);
            }

            std::string result;
            if (timeFormat == TimeFormat::ShortText)
            {
                std::back_insert_iterator<std::string> itr = std::back_inserter(result);
                if (days)
                    StringFormatTo(itr, "{}d", days);
                if (hours || hoursOnly)
                    StringFormatTo(itr, "{}h", hours);
                if (!hoursOnly)
                {
                    if (minutes)
                        StringFormatTo(itr, "{}m", minutes);
                    if (secs || result.empty())
                        StringFormatTo(itr, "{}s", secs);
                }
            }
            else if (timeFormat == TimeFormat::FullText)
            {
                auto formatTimeField = [](std::string& result, uint64 value, std::string_view label)
                {
                    if (!result.empty())
                        result.append(1, ' ');
                    StringFormatTo(std::back_inserter(result), "{} {}", value, label);
                    if (value != 1)
                        result.append(1, 's');
                };
                if (days)
                    formatTimeField(result, days, "Day");
                if (hours || hoursOnly)
                    formatTimeField(result, hours, "Hour");
                if (!hoursOnly)
                {
                    if (minutes)
                        formatTimeField(result, minutes, "Minute");
                    if (secs || result.empty())
                        formatTimeField(result, secs, "Second");
                }
                result.append(1, '.');
            }
            else
                result = "<Unknown time format>";

            return result;
        }

        uint32 TimeStringToSecs(std::string const& timestring)
        {
            uint32 secs = 0;
            uint32 buffer = 0;
            uint32 multiplier = 0;

            for (char itr : timestring)
            {
                if (isdigit(itr))
                {
                    buffer *= 10;
                    buffer += itr - '0';
                }
                else
                {
                    switch (itr)
                    {
                    case 'd': multiplier = DAY;
                        break;
                    case 'h': multiplier = HOUR;
                        break;
                    case 'm': multiplier = MINUTE;
                        break;
                    case 's': multiplier = 1;
                        break;
                    default: return 0; //bad format
                    }
                    buffer *= multiplier;
                    secs += buffer;
                    buffer = 0;
                }
            }

            return secs;
        }

        std::string TimeToTimestampStr(time_t t)
        {
            tm aTm;
            localtime_r(&t, &aTm);
            //       YYYY   year
            //       MM     month (2 digits 01-12)
            //       DD     day (2 digits 01-31)
            //       HH     hour (2 digits 00-23)
            //       MM     minutes (2 digits 00-59)
            //       SS     seconds (2 digits 00-59)
            return StringFormat("{:04}-{:02}-{:02}_{:02}-{:02}-{:02}", aTm.tm_year + 1900, aTm.tm_mon + 1,
                                         aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec);
        }

        std::string TimeToHumanReadable(time_t t)
        {
            tm time;
            localtime_r(&t, &time);
            char buf[30];
            strftime(buf, 30, "%c", &time);
            return std::string(buf);
        }
    }

END_NAMESPACE_COMMON
