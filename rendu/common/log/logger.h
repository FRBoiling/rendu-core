/*
* Created by boil on 2022/12/31.
*/

#ifndef RENDU_CORE_LOGGER_H_
#define RENDU_CORE_LOGGER_H_
#include "spdlog/spdlog.h"

class logger {
};
#define RD_LOG_TRACE(...) SPDLOG_TRACE(...);
#define RD_LOG_DEBUG(...) SPDLOG_DEBUG(...);
#define RD_LOG_INFO(...) SPDLOG_INFO(...);
#define RD_LOG_WARN(...) SPDLOG_WARN(...);
#define RD_LOG_ERROR(...) SPDLOG_ERROR(...);
#define RD_LOG_CRITICAL(...) SPDLOG_CRITICAL(...);
#endif //RENDU_CORE_LOGGER_H_
