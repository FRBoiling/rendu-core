/*
* Created by boil on 2023/11/14.
*/

#ifndef RENDU_LOG_H
#define RENDU_LOG_H

#include "console/console_channel.h"
#include "log/async_log_writer.h"
#include "logger_wrapper.h"
#include "async_logger.h"

RD_NAMESPACE_BEGIN

//用法: PrintD("%d + %s = %c", 1 "2", 'c');
#define PrintLog(level, ...) LoggerWrapper::PrintLog(GetLogger(), level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define PrintT(...) PrintLog(LTrace, ##__VA_ARGS__)
#define PrintD(...) PrintLog(LDebug, ##__VA_ARGS__)
#define PrintI(...) PrintLog(LInfo, ##__VA_ARGS__)
#define PrintW(...) PrintLog(LWarn, ##__VA_ARGS__)
#define PrintE(...) PrintLog(LError, ##__VA_ARGS__)

//用法: LogD(1, "+", "2", '=', 3);
//用于模板实例化的原因，如果每次打印参数个数和类型不一致，可能会导致二进制代码膨胀
#define LogL(level, ...) LoggerWrapper::printLogArray(GetLogger(), (LogLevel::Level) level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LogT(...) LogL(LTrace, ##__VA_ARGS__)
#define LogD(...) LogL(LDebug, ##__VA_ARGS__)
#define LogI(...) LogL(LInfo, ##__VA_ARGS__)
#define LogW(...) LogL(LWarn, ##__VA_ARGS__)
#define LogE(...) LogL(LError, ##__VA_ARGS__)

RD_NAMESPACE_END

#endif//RENDU_LOG_H
