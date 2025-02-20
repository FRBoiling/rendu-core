/*
* Created by boil on 2024/8/7.
*/

#ifndef RENDU_BASIC_BASIC_LOG_LOG_MSG_H_
#define RENDU_BASIC_BASIC_LOG_LOG_MSG_H_


#include "log_define.h"
#include "utils/string_format.h"

RD_NAMESPACE_BEGIN

class LogMsg {
public:
  LogMsg(){};

  ~LogMsg(){};
private:
  String content_;
public:
  template<typename... Args>
  void setMsg(StringView format_string, Args &&...args){
     content_ = format(format_string, args...);
  }

  void setMsg(StringView msg){
    content_ = msg.data();
  }

  String getMsg() const{
    return content_;
  }
};

RD_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_LOG_LOG_MSG_H_
