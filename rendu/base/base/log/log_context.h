/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_BASE_LOG_CONTEXT_H
#define RENDU_BASE_LOG_CONTEXT_H

#include "log_level.h"

#include <sstream>
#include "number/number_helper.h"
#include "string/string_helper.h"

RD_NAMESPACE_BEGIN
/**
* 日志上下文
*/
  class LogContext : public std::ostringstream {
  public:
    using Ptr = std::shared_ptr<LogContext>;
  public:
    //_file,_function改成STRING保存，目的是有些情况下，指针可能会失效
    //比如说动态库中打印了一条日志，然后动态库卸载了，那么指向静态数据区的指针就会失效

    LogContext() : _level(LogLevel::Level::LInvalid), _line(0), _repeat(true), _file(""), _function(""), _module_name(""),
                   _flag("") {

    }

    LogContext(LogLevel::Level level,
               const char *file,
               const char *function,
               int line,
               const char *module_name,
               const char *flag);

    ~LogContext() = default;

  public:
    const STRING &str();

  public:
    LogLevel::Level GetLogLevel() { return _level; }

    void SetRepeat(int repeat) { _repeat = repeat; }

    struct timeval& GetTimeval() { return _tv; }

    STRING GetFlag() { return _flag; }
    STRING GetFile() { return _file; }
    STRING GetFunction() { return _function; }
    STRING GetThreadName() { return _thread_name; }
    STRING GetModuleName() { return _module_name; }
    INT32 GetLine() { return _line; }
    INT32 GetRepeat() { return _repeat; }

  private:
    LogLevel::Level _level;
    INT32 _line;
    INT32 _repeat;
    STRING _file;
    STRING _function;
    STRING _thread_name;
    STRING _module_name;
    STRING _flag;
    struct timeval _tv;

  private:
    bool _got_content = false;
    STRING _content;
  };

RD_NAMESPACE_END

#endif //RENDU_BASE_LOG_CONTEXT_H
