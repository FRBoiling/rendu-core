/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_BASE_A_LOG_CHANNEL_H
#define RENDU_BASE_A_LOG_CHANNEL_H

#include "log_define.h"
#include "a_logger.h"

RD_NAMESPACE_BEGIN
/**
 * 日志通道
 */
class ALogChannel : public NonCopyable {
public:
  using Ptr = std::shared_ptr<ALogChannel>;

public:
  ALogChannel(const STRING &name = "default", LogLevel::Level level = LogLevel::LInvalid);

  virtual ~ALogChannel();

public:
  virtual void Write(const LogContext::Ptr &ctx, ALogger &logger) ;

public:
  const STRING &Name() const;

  void SetLevel(LogLevel::Level level);

  static STRING PrintTime(const timeval &tv);

protected:
  /**
    * 打印日志至输出流
    * @param ost 输出流
    * @param enable_color 是否启用颜色
    * @param enable_detail 是否打印细节(函数名、源码文件名、源码行)
    */
  virtual void Format(const ALogger &logger, std::ostream &ost, const LogContext::Ptr& ctx);

protected:
  STRING _name;
  LogLevel::Level _level;
  bool _enable_detail;
};

RD_NAMESPACE_END

#endif//RENDU_BASE_A_LOG_CHANNEL_H
