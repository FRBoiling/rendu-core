/*
* Created by boil on 2024/8/9.
*/

#ifndef RENDU_BASIC_BASIC_LOG_LOG_MODE_H_
#define RENDU_BASIC_BASIC_LOG_LOG_MODE_H_

#include "log_define.h"

RD_NAMESPACE_BEGIN

enum LogMode {
  LM_SYNC, //同步模式
  LM_ASYNC,//异步模式
};

RD_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_LOG_LOG_MODE_H_
