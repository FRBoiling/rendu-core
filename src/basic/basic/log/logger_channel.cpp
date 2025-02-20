/*
* Created by boil on 2024/8/9.
*/

#include "logger_channel.h"
#include "logger.h"

RD_NAMESPACE_BEGIN

LoggerChannel::LoggerChannel(Logger *logger, String name) : m_logger(logger), m_name(name) {
}

void LoggerChannel::write(LogLevel level, LogMsgSource &prefix, const LogMsg &content) {
  printf("%s[%s][%s,%s:%d] %s\n",
         m_logger->getFlag().c_str(),
         GetLogLevelSimpleName(level),
         prefix.getFunctionName(),
         prefix.getFileName(),
         prefix.getLineNum(),
         content.getMsg().c_str());
}

RD_NAMESPACE_END