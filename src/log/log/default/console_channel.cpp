/*
* Created by boil on 2023/12/21.
*/

#include "console_channel.h"
#include "console_color.h"
#include "log_msg.h"

LOG_NAMESPACE_BEGIN

ConsoleChannel::ConsoleChannel() : ALoggerChannel() {
}
void ConsoleChannel::WriteMsg(const LogLevel level, const LogMsgSource &prefix, const std::string &content) {
  LogMsg log_msg{LogMsgTime{},prefix,m_logger->GetFlag(),level,content};
  int index = 1;
  //  if (level == LogLevel::LL_CRITICAL){
  //    index = 0;
  //  }
  auto color = ConsoleColor::SET_COLOR(level,index);
  printf("%s%s%s\n", color,log_msg.ToString().c_str(),CLEAR_COLOR);
}

LOG_NAMESPACE_END

