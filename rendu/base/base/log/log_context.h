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

    LogContext();

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

    std::string GetFlag() { return _flag; }
    std::string GetFile() { return _file; }
    std::string GetFunction() { return _function; }
    std::string GetThreadName() { return _thread_name; }
    std::string GetModuleName() { return _module_name; }
    std::int32_t GetLine() { return _line; }
    std::int32_t GetRepeat() { return _repeat; }

  private:
    LogLevel::Level _level;
    std::int32_t _line;
    std::int32_t _repeat;
    std::string _file;
    std::string _function;
    std::string _thread_name;
    std::string _module_name;
    std::string _flag;
    struct timeval _tv{};

  private:
    bool _got_content = false;
    std::string _content;
  };

RD_NAMESPACE_END

#endif //RENDU_BASE_LOG_CONTEXT_H
