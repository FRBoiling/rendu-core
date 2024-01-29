/*
* Created by boil on 2023/12/23.
*/

#include "log_msg.h"

LOG_NAMESPACE_BEGIN

size_t GetCurrThreadId(){
  return 1;
}

LogMsg::LogMsg(LogMsgTime log_time, LogMsgSource source, string_view_t logger_name, LogLevel lvl, string_view_t content)
    :time_(log_time),source_(source),logger_name_(logger_name),lvl_(lvl),content_(content) ,thread_id_(GetCurrThreadId())
                                                                                  {

}


LOG_NAMESPACE_END