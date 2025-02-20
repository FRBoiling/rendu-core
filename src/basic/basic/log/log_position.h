/*
* Created by boil on 2024/8/9.
*/

#ifndef RENDU_BASIC_BASIC_LOG_LOG_POSITION_H_
#define RENDU_BASIC_BASIC_LOG_LOG_POSITION_H_

#include "log_define.h"

RD_NAMESPACE_BEGIN

//日志输出位置
enum LogPosition {
  LP_CONSOLE = 0x01,         //控制台
  LP_FILE = 0X02,            //文件
  LP_CONSOLE_AND_FILE = 0x03,//控制台+文件
};

RD_NAMESPACE_END


#endif//RENDU_BASIC_BASIC_LOG_LOG_POSITION_H_
