/*
* Created by boil on 2023/11/14.
*/

#ifndef RENDU_LOG_H
#define RENDU_LOG_H

#include "base/log/logger_wrapper.h"
#include "log/async/async_log_writer.h"
#include "log/async/async_logger.h"
#include "log/async/console_channel.h"
#include "log/spdlog/spdlog_logger.h"

LOG_NAMESPACE_BEGIN

#define RD_LOGGER ((SpdlogLogger*)LOG_GET_LOGGER())
#define RD_STOPWATCH spdlog::stopwatch

//封装宏，没有该宏无法输出文件名、行号等信息
#define RD_TRACE(...) SPDLOG_LOGGER_CALL(RD_LOGGER->getLogger(), spdlog::level::trace, __VA_ARGS__)
#define RD_DEBUG(...) SPDLOG_LOGGER_CALL(RD_LOGGER->getLogger(), spdlog::level::debug, __VA_ARGS__)
#define RD_INFO(...) SPDLOG_LOGGER_CALL(RD_LOGGER->getLogger(), spdlog::level::info, __VA_ARGS__)
#define RD_WARN(...) SPDLOG_LOGGER_CALL(RD_LOGGER->getLogger(), spdlog::level::warn, __VA_ARGS__)
#define RD_ERROR(...) SPDLOG_LOGGER_CALL(RD_LOGGER->getLogger(), spdlog::level::err, __VA_ARGS__)
#define RD_CRITICAL(...) SPDLOG_LOGGER_CALL(RD_LOGGER->getLogger(), spdlog::level::critical, __VA_ARGS__)

LOG_NAMESPACE_END

#endif//RENDU_LOG_H
