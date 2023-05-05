/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_LOGGER_H
#define RENDU_LOGGER_H

#include <spdlog/spdlog.h>

#define RD_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define RD_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define RD_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define RD_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define RD_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define RD_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

#endif //RENDU_LOGGER_H
