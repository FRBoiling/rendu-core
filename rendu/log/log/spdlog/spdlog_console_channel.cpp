/*
* Created by boil on 2023/11/24.
*/

#include "spdlog_console_channel.h"

LOG_NAMESPACE_BEGIN


SpdlogConsoleChannel::SpdlogConsoleChannel(const std::string &name, LogLevel::Level level, bool enable_color) : ALogChannel(name, level),
                                                                                                                enable_color_(enable_color) {}

void SpdlogConsoleChannel::Write(const LogContext::Ptr &ctx, ALogger &logger) {



}

void SpdlogConsoleChannel::Format(const ALogger &logger, std::ostream &ost, const LogContext::Ptr &ctx) {
}


LOG_NAMESPACE_END