/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_LOG_CONTEXT_H
#define RENDU_LOG_CONTEXT_H

#include "log_level.h"
#include <sstream>

RD_NAMESPACE_BEGIN

  class Logger;

/**
* 日志上下文
*/
  class LogContext : public std::ostringstream {
  public:
    using Ptr = std::shared_ptr<LogContext>;
  public:
    //_file,_function改成string保存，目的是有些情况下，指针可能会失效
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
    const std::string &str();

  public:
    LogLevel::Level GetLogLevel() { return _level; }

    void SetRepeat(int repeat) { _repeat = repeat; }

    struct timeval& GetTimeval() { return _tv; }

    string GetFlag() { return _flag; }
    string GetFile() { return _file; }
    string GetFunction() { return _function; }
    string GetThreadName() { return _thread_name; }
    string GetModuleName() { return _module_name; }
    int32 GetLine() { return _line; }
    int32 GetRepeat() { return _repeat; }

  private:
    LogLevel::Level _level;
    int32 _line;
    int32 _repeat;
    std::string _file;
    std::string _function;
    std::string _thread_name;
    std::string _module_name;
    std::string _flag;
    struct timeval _tv;


  private:
    bool _got_content = false;
    std::string _content;
  };

RD_NAMESPACE_END

#endif //RENDU_LOG_CONTEXT_H
