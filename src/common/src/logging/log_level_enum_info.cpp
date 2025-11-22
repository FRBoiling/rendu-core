//
// Created by 沸腾 on 2025/11/22.
//


#include "common/logging/log_level.h"
#include "common/utils/enum_utils.h"
#include <stdexcept>

BEGIN_NAMESPACE_COMMON
    using namespace Logging;

    namespace Utils::Impl::EnumUtilsImpl
    {
        template <>
        RC_API_EXPORT EnumText EnumUtils<LogLevel>::ToString(LogLevel value)
        {
            switch (value)
            {
            case LOG_LEVEL_DISABLED: return {"LOG_LEVEL_DISABLED", "LOG_LEVEL_DISABLED", ""};
            case LOG_LEVEL_TRACE: return {"LOG_LEVEL_TRACE", "LOG_LEVEL_TRACE", ""};
            case LOG_LEVEL_DEBUG: return {"LOG_LEVEL_DEBUG", "LOG_LEVEL_DEBUG", ""};
            case LOG_LEVEL_INFO: return {"LOG_LEVEL_INFO", "LOG_LEVEL_INFO", ""};
            case LOG_LEVEL_WARN: return {"LOG_LEVEL_WARN", "LOG_LEVEL_WARN", ""};
            case LOG_LEVEL_ERROR: return {"LOG_LEVEL_ERROR", "LOG_LEVEL_ERROR", ""};
            case LOG_LEVEL_FATAL: return {"LOG_LEVEL_FATAL", "LOG_LEVEL_FATAL", ""};
            default: throw std::out_of_range("value");
            }
        }

        template <>
        RC_API_EXPORT size_t EnumUtils<LogLevel>::Count() { return 7; }

        template <>
        RC_API_EXPORT LogLevel EnumUtils<LogLevel>::FromIndex(size_t index)
        {
            switch (index)
            {
            case 0: return LOG_LEVEL_DISABLED;
            case 1: return LOG_LEVEL_TRACE;
            case 2: return LOG_LEVEL_DEBUG;
            case 3: return LOG_LEVEL_INFO;
            case 4: return LOG_LEVEL_WARN;
            case 5: return LOG_LEVEL_ERROR;
            case 6: return LOG_LEVEL_FATAL;
            default: throw std::out_of_range("index");
            }
        }

        template <>
        RC_API_EXPORT size_t EnumUtils<LogLevel>::ToIndex(LogLevel value)
        {
            switch (value)
            {
            case LOG_LEVEL_DISABLED: return 0;
            case LOG_LEVEL_TRACE: return 1;
            case LOG_LEVEL_DEBUG: return 2;
            case LOG_LEVEL_INFO: return 3;
            case LOG_LEVEL_WARN: return 4;
            case LOG_LEVEL_ERROR: return 5;
            case LOG_LEVEL_FATAL: return 6;
            default: throw std::out_of_range("value");
            }
        }
    } // namespace Logging

END_NAMESPACE_COMMON
