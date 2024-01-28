/*
* Created by boil on 2023/12/21.
*/

#include "console_channel.h"
#include "console_color.h"

LOG_NAMESPACE_BEGIN

using namespace common;
ConsoleChannel::ConsoleChannel() : ALoggerChannel() {
}
void ConsoleChannel::WriteMsg(LogLevel level, LogMsgSource &source, std::string &content) {
  LogMsg log_msg{LogMsgTime{},source,logger_->GetFlag(),level,content};
  int index = 1;
//  if (level == LogLevel::LL_CRITICAL){
//    index = 0;
//  }
  auto color = ConsoleColor::SET_COLOR(level,index);
  printf("%s%s%s\n", color,log_msg.ToString().c_str(),CLEAR_COLOR);
}

LOG_NAMESPACE_END
