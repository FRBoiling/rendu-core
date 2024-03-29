/*
* Created by boil on 2023/10/26.
*/

#include <sys/time.h>
#include "log_context.h"
#include "thread/thread_helper.h"

RD_NAMESPACE_BEGIN

  static inline const char *GetFileName(const char *file) {
    auto pos = strrchr(file, '/');
#ifdef _WIN32
    if(!pos){
          pos = strrchr(file, '\\');
      }
#endif
    return pos ? pos + 1 : file;
  }

  static inline const char *GetFunctionName(const char *func) {
#ifndef _WIN32
    return func;
#else
    auto pos = strrchr(func, ':');
      return pos ? pos + 1 : func;
#endif
  }

  LogContext::LogContext(LogLevel::Level level, const char *file, const char *function, int line, const char *module_name,
                         const char *flag)
    : _level(level), _line(line), _file(GetFileName(file)), _function(GetFunctionName(function)),
      _module_name(module_name), _flag(flag) {
    gettimeofday(&_tv, nullptr);
    _thread_name = ThreadHelper::GetThreadName();
  }

  const string &LogContext::str() {
    if (_got_content) {
      return _content;
    }
    _content = std::ostringstream::str();
    _got_content = true;
    return _content;
  }


RD_NAMESPACE_END