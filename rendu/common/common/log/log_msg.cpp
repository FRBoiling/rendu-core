/*
* Created by boil on 2023/12/23.
*/

#include "log_msg.h"
#include "base/thread/thread_helper.h"

COMMON_NAMESPACE_BEGIN

LogMsg::LogMsg(LogMsgTime log_time, LogMsgSource source, string_view_t logger_name, LogLevel lvl, string_view_t content)
    :time_(log_time),source_(source),logger_name_(logger_name),lvl_(lvl),content_(content) ,thread_id_(GetCurrThreadId())
                                                                                  {

}


COMMON_NAMESPACE_END