/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_LOGGER_H
#define RENDU_LOGGER_H

#include "utils/noncopyable.h"
#include <map>
#include "log_context_capture.h"
#include "a_log_channel.h"
#include "i_log_writer.h"

RD_NAMESPACE_BEGIN

#ifdef _WIN32
#define CLEAR_COLOR 7
  static const WORD LOG_CONST_TABLE[][3] = {
          {0x97, 0x09 , 'T'},//蓝底灰字，黑底蓝字，window console默认黑底
          {0xA7, 0x0A , 'D'},//绿底灰字，黑底绿字
          {0xB7, 0x0B , 'I'},//天蓝底灰字，黑底天蓝字
          {0xE7, 0x0E , 'W'},//黄底灰字，黑底黄字
          {0xC7, 0x0C , 'E'} };//红底灰字，黑底红字

  bool SetConsoleColor(WORD Color)
  {
      HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
      if (handle == 0)
          return false;

      BOOL ret = SetConsoleTextAttribute(handle, Color);
      return(ret == TRUE);
  }
#else
#define CLEAR_COLOR "\033[0m"
  static const char *LOG_CONST_TABLE[][3] = {
    {"\033[44;37m", "\033[34m", "T"},
    {"\033[42;37m", "\033[32m", "D"},
    {"\033[46;37m", "\033[36m", "I"},
    {"\033[43;37m", "\033[33m", "W"},
    {"\033[41;37m", "\033[31m", "E"}};
#endif

/**
* 日志类
*/
  class Logger : public std::enable_shared_from_this<Logger>, public noncopyable {
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
    void SetLevel(LogLevel level);

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

//用法: DebugL << 1 << "+" << 2 << '=' << 3;
#define WriteL(level) LogContextCapture(GetLogger(), level, __FILE__, __FUNCTION__, __LINE__)
#define TraceL WriteL(LTrace)
#define DebugL WriteL(LDebug)
#define InfoL WriteL(LInfo)
#define WarnL WriteL(LWarn)
#define ErrorL WriteL(LError)

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
#define LogL(level, ...) LoggerWrapper::printLogArray(getLogger(), (LogLevel)level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LogT(...) LogL(LTrace, ##__VA_ARGS__)
#define LogD(...) LogL(LDebug, ##__VA_ARGS__)
#define LogI(...) LogL(LInfo, ##__VA_ARGS__)
#define LogW(...) LogL(LWarn, ##__VA_ARGS__)
#define LogE(...) LogL(LError, ##__VA_ARGS__)

RD_NAMESPACE_END

#endif //RENDU_LOGGER_H
