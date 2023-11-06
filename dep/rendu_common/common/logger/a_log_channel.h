/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_A_LOG_CHANNEL_H
#define RENDU_A_LOG_CHANNEL_H

#include "log_context.h"
#include "utils/non_copyable.h"

RD_NAMESPACE_BEGIN

/**
 * 日志通道
 */
  class ALogChannel : public NonCopyable {
  public:
    ALogChannel(const std::string &name, LogLevel::Level level = LogLevel::LInvalid);

    virtual ~ALogChannel();

    virtual void Write(const Logger &logger, const LogContext::Ptr &ctx) = 0;

  public:
    const std::string &Name() const;

    void SetLevel(LogLevel::Level level);

    static std::string PrintTime(const timeval &tv);

  protected:
    /**
    * 打印日志至输出流
    * @param ost 输出流
    * @param enable_color 是否启用颜色
    * @param enable_detail 是否打印细节(函数名、源码文件名、源码行)
    */
    virtual void Format(const Logger &logger, std::ostream &ost, const LogContext::Ptr &ctx);

  protected:
    std::string _name;
    LogLevel::Level _level;
    bool _enable_detail;
  };

RD_NAMESPACE_END

#endif //RENDU_A_LOG_CHANNEL_H
