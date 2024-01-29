/*
* Created by boil on 2023/12/21.
*/

#include "a_logger_channel.h"
#include "log_msg.h"
#include <sstream>

LOG_NAMESPACE_BEGIN

ALoggerChannel::ALoggerChannel() : logger_(nullptr) {

}

void ALoggerChannel::Init(Logger *logger) {
  logger_ = logger;
}


void ALoggerChannel::WriteMsg(LogLevel level, LogMsgSource &source, std::string &content) {
  LogMsg log_msg{LogMsgTime{},source,"default",level,content};
  printf("%s\n", log_msg.ToString().c_str());
}

LOG_NAMESPACE_END