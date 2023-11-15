/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_BASE_A_LOGGER_H
#define RENDU_BASE_A_LOGGER_H

#include "container/container_helper.h"
#include "log_context.h"
#include "log_fwd.h"

RD_NAMESPACE_BEGIN
/**
* 日志类
*/
class ALogger : public std::enable_shared_from_this<ALogger>, public NonCopyable {

public:
  explicit ALogger(const std::string &loggerName);

  ~ALogger();

public:
  /**
     * 获取日志通道，非线程安全的
     * @param name log通道名
     * @return 线程通道
     */
  std::shared_ptr<ALogChannel> Get(const std::string &name);

  /**
     * 添加日志通道，非线程安全的
     * @param channel log通道
     */
  void Add(const std::shared_ptr<ALogChannel> &channel);
  /**
     * 删除日志通道，非线程安全的
     * @param name log通道名
     */
  void Remove(const std::string &name);


  /**
     * 设置写log器，非线程安全的
     * @param writer 写log器
     */
  void SetWriter(const std::shared_ptr<ALogWriter> &writer);

  /**
     * 设置所有日志通道的log等级
     * @param level log等级
     */
  void SetLevel(LogLevel::Level level);

  /**
     * 获取logger名
     * @return logger名
     */
  const std::string &GetName() const;

  /**
     * 写日志
     * @param ctx 日志信息
     */
  virtual void Write(const LogContext::Ptr& ctx);

protected:
  std::string _logger_name;
  std::shared_ptr<ALogWriter> _writer;
  std::map<std::string, std::shared_ptr<ALogChannel>> _channels;
};

RD_NAMESPACE_END

#endif//RENDU_BASE_A_LOGGER_H
