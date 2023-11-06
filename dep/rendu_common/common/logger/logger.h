/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_LOGGER_H
#define RENDU_LOGGER_H

#include "utils/non_copyable.h"
#include <map>
#include "log_context_capture.h"
#include "a_log_channel.h"
#include "i_log_writer.h"

RD_NAMESPACE_BEGIN

/**
* 日志类
*/
  class Logger : public std::enable_shared_from_this<Logger>, public NonCopyable {
  public:
    friend class AsyncLogWriter;

    using Ptr = std::shared_ptr<Logger>;

    /**
     * 获取日志单例
     * @return
     */
    static Logger &Instance();

    explicit Logger(const std::string &loggerName);

    ~Logger();

  public:
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
     * 获取日志通道，非线程安全的
     * @param name log通道名
     * @return 线程通道
     */
    std::shared_ptr<ALogChannel> Get(const std::string &name);

    /**
     * 设置写log器，非线程安全的
     * @param writer 写log器
     */
    void SetWriter(const std::shared_ptr<ILogWriter> &writer);

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
    void Write(const LogContext::Ptr &ctx);

  private:
    /**
     * 写日志到各channel，仅供AsyncLogWriter调用
     * @param ctx 日志信息
     */
    void writeChannels(const LogContext::Ptr &ctx);

    void writeChannels_l(const LogContext::Ptr &ctx);

  private:
    LogContext::Ptr _last_log;
    std::string _logger_name;
    std::shared_ptr<ILogWriter> _writer;
    std::map<std::string, std::shared_ptr<ALogChannel> > _channels;
  };

  Logger &GetLogger();

  void SetLogger(Logger *logger);

//可重置默认值
  extern Logger *g_defaultLogger;

//用法: LOG_DEBUG << 1 << "+" << 2 << '=' << 3;
#define LOG_Write(level) LogContextCapture(GetLogger(), level, __FILE__, __FUNCTION__, __LINE__)
#define LOG_TRACE LOG_Write(LogLevel::LTrace)
#define LOG_DEBUG LOG_Write(LogLevel::LDebug)
#define LOG_INFO LOG_Write(LogLevel::LInfo)
#define LOG_WARN LOG_Write(LogLevel::LWarn)
#define LOG_ERROR LOG_Write(LogLevel::LError)
#define LOG_CRITICAL LOG_Write(LogLevel::LCritical)

//只能在虚继承BaseLogFlagInterface的类中使用
#define WriteF(level) LogContextCapture(GetLogger(), level, __FILE__, __FUNCTION__, __LINE__, getLogFlag())
#define TraceF WriteF(LTrace)
#define DebugF WriteF(LDebug)
#define InfoF WriteF(LInfo)
#define WarnF WriteF(LWarn)
#define ErrorF WriteF(LError)

//用法: PrintD("%d + %s = %c", 1 "2", 'c');
#define PrintLog(level, ...) LoggerWrapper::PrintLog(GetLogger(), level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define PrintT(...) PrintLog(LTrace, ##__VA_ARGS__)
#define PrintD(...) PrintLog(LDebug, ##__VA_ARGS__)
#define PrintI(...) PrintLog(LInfo, ##__VA_ARGS__)
#define PrintW(...) PrintLog(LWarn, ##__VA_ARGS__)
#define PrintE(...) PrintLog(LError, ##__VA_ARGS__)

//用法: LogD(1, "+", "2", '=', 3);
//用于模板实例化的原因，如果每次打印参数个数和类型不一致，可能会导致二进制代码膨胀
#define LogL(level, ...) LoggerWrapper::printLogArray(getLogger(), (LogLevel::Level)level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LogT(...) LogL(LTrace, ##__VA_ARGS__)
#define LogD(...) LogL(LDebug, ##__VA_ARGS__)
#define LogI(...) LogL(LInfo, ##__VA_ARGS__)
#define LogW(...) LogL(LWarn, ##__VA_ARGS__)
#define LogE(...) LogL(LError, ##__VA_ARGS__)

RD_NAMESPACE_END

#endif //RENDU_LOGGER_H
