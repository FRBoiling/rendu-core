/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_BASE_LOG_CONTEXT_CAPTURE_H
#define RENDU_BASE_LOG_CONTEXT_CAPTURE_H

#include "log_context.h"
#include "a_logger.h"

RD_NAMESPACE_BEGIN

/**
 * 日志上下文捕获器
 */
  class LogContextCapture {
  public:

    LogContextCapture(ALogger* logger, LogLevel::Level level, const char *file, const char *function, int line, const char *flag = "##");

    LogContextCapture(const LogContextCapture &that);

    ~LogContextCapture();
  public:
    using Ptr = std::shared_ptr<LogContextCapture>;

    /**
     * 输入std::endl(回车符)立即输出日志
     * @param f std::endl(回车符)
     * @return 自身引用
     */
    LogContextCapture &operator<<(std::ostream &(*f)(std::ostream &));

    template<typename T>
    LogContextCapture &operator<<(T &&data) {
      if (!_logger_ctx) {
        return *this;
      }
      (*_logger_ctx) << std::forward<T>(data);
      return *this;
    }

  public:
    void Clear();

  protected:
    LogContext::Ptr _logger_ctx;
    ALogger* _logger;
  };

RD_NAMESPACE_END

#endif //RENDU_BASE_LOG_CONTEXT_CAPTURE_H
