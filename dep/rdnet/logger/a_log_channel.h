/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_A_LOG_CHANNEL_H
#define RENDU_A_LOG_CHANNEL_H

#include "log_context.h"
#include "utils/noncopyable.h"

RD_NAMESPACE_BEGIN

/**
 * 日志通道
 */
  class ALogChannel : public noncopyable {
  public:
    ALogChannel(const std::string &name, LogLevel level = LTrace);

    virtual ~ALogChannel();

    virtual void write(const Logger &logger, const LogContext::Ptr &ctx) = 0;

    const std::string &name() const;

    void setLevel(LogLevel level);

    static std::string printTime(const timeval &tv);

  protected:
    /**
    * 打印日志至输出流
    * @param ost 输出流
    * @param enable_color 是否启用颜色
    * @param enable_detail 是否打印细节(函数名、源码文件名、源码行)
    */
    virtual void Format(const Logger &logger, std::ostream &ost, const LogContext::Ptr &ctx, bool enable_color = true,
                        bool enable_detail = true);

  protected:
    std::string _name;
    LogLevel _level;
  };

RD_NAMESPACE_END

#endif //RENDU_A_LOG_CHANNEL_H
