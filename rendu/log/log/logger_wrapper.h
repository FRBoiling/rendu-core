/*
* Created by boil on 2023/10/28.
*/

#ifndef RENDU_LOGGER_WRAPPER_H
#define RENDU_LOGGER_WRAPPER_H

#include "logger.h"

LOG_NAMESPACE_BEGIN

  class LoggerWrapper {
  public:
    template<typename First, typename ...ARGS>
    static inline void
    PrintLogArray(Logger &logger, LogLevel::Level level, const char *file, const char *function, int line, First &&first,
                  ARGS &&...args) {
      LogContextCapture log(logger, level, file, function, line);
      log << std::forward<First>(first);
      appendLog(log, std::forward<ARGS>(args)...);
    }

    static inline void PrintLogArray(Logger &logger, LogLevel::Level level, const char *file, const char *function, int line) {
      LogContextCapture log(logger, level, file, function, line);
    }

    template<typename Log, typename First, typename ...ARGS>
    static inline void AppendLog(Log &out, First &&first, ARGS &&...args) {
      out << std::forward<First>(first);
      appendLog(out, std::forward<ARGS>(args)...);
    }

    template<typename Log>
    static inline void AppendLog(Log &out) {}

    //printf样式的日志打印
    static void
    printLog(Logger &logger, int level, const char *file, const char *function, int line, const char *fmt, ...);

    static void
    printLogV(Logger &logger, int level, const char *file, const char *function, int line, const char *fmt, va_list ap);
  };

LOG_NAMESPACE_END

#endif //RENDU_LOGGER_WRAPPER_H
