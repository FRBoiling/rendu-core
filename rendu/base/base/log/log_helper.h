/*
* Created by boil on 2023/11/14.
*/

#ifndef RENDU_BASE_LOG_HELPER_H
#define RENDU_BASE_LOG_HELPER_H

#include "log_context_capture.h"
#include "log_define.h"

#include "a_log_channel.h"
#include "a_log_writer.h"

RD_NAMESPACE_BEGIN

//可重置默认值
extern ALogger *g_defaultLogger;

extern ALogger *GetLogger();

extern void SetLogger(ALogger *logger);


#define LOG_SET_LOGGER(logger) SetLogger(logger)
#define LOG_GET_LOGGER() GetLogger()

//用法: LOG_DEBUG << 1 << "+" << 2 << '=' << 3;
#define LOG_Write(level) LogContextCapture(LOG_GET_LOGGER(), level, __FILE__, __FUNCTION__, __LINE__)
#define LOG_TRACE LOG_Write(LogLevel::LTrace)
#define LOG_DEBUG LOG_Write(LogLevel::LDebug)
#define LOG_INFO LOG_Write(LogLevel::LInfo)
#define LOG_WARN LOG_Write(LogLevel::LWarn)
#define LOG_ERROR LOG_Write(LogLevel::LError)
#define LOG_CRITICAL LOG_Write(LogLevel::LCritical)

//只能在虚继承BaseLogFlagInterface的类中使用
#define WriteF(level) LogContextCapture(LOG_GET_LOGGER(), level, __FILE__, __FUNCTION__, __LINE__, getLogFlag())
#define TraceF WriteF(LTrace)
#define DebugF WriteF(LDebug)
#define InfoF WriteF(LInfo)
#define WarnF WriteF(LWarn)
#define ErrorF WriteF(LError)

RD_NAMESPACE_END


#endif//RENDU_BASE_LOG_HELPER_H
