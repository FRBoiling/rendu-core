/*
* Created by boil on 2024/8/7.
*/

#ifndef RENDU_BASIC_BASIC_LOG_LOG_MSG_SOURCE_H_
#define RENDU_BASIC_BASIC_LOG_LOG_MSG_SOURCE_H_


#include "log_define.h"

RD_NAMESPACE_BEGIN

class LogMsgSource {
  public:
    LogMsgSource(const char *filename, int line, const char *function_name)
        :_filename{filename},_line(line),_function_name(function_name) {};
    ~LogMsgSource()= default;
  private:
    const char *_filename{nullptr};
    int _line{0};
    const char *_function_name{nullptr};
  public:
    inline const char* getFileName() const {return _filename;}
    inline const char* getSimpleFileName() const {return _filename;}
    inline int getLineNum() const { return _line; }
    inline const char*  getFunctionName() const { return _function_name; }
};

RD_NAMESPACE_END


#endif//RENDU_BASIC_BASIC_LOG_LOG_MSG_SOURCE_H_
