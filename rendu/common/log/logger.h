/*
* Created by boil on 2022/12/31.
*/

#ifndef RENDU_CORE_LOGGER_H_
#define RENDU_CORE_LOGGER_H_
#include "spdlog/spdlog.h"

class logger {
};
#define RD_LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define RD_LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define RD_LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define RD_LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define RD_LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define RD_LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
#endif //RENDU_CORE_LOGGER_H_
