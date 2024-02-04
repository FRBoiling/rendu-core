/*
* Created by boil on 2023/12/21.
*/

#include "a_logger_channel.h"
#include "a_logger.h"
#include <sstream>

RD_NAMESPACE_BEGIN

ALoggerChannel::ALoggerChannel() :m_name("ALoggerChannel"){
}

void ALoggerChannel::Init(ALogger *logger) {
  m_logger = logger;
}

void ALoggerChannel::WriteMsg(const LogLevel level, const LogMsgSource &prefix, const std::string &content) {
  printf("%s[%s][%s,%s:%d] %s\n", m_logger->GetFlag().c_str(), GetLogLevelSimpleName(level), prefix.GetFunctionName(), prefix.GetFileName(), prefix.GetLine(), content.c_str());
}

RD_NAMESPACE_END