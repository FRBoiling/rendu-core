/*
* Created by boil on 2024/7/11.
*/

#ifndef RENDU_TIMER_TIMER_TIMER_DEFINE_H_
#define RENDU_TIMER_TIMER_TIMER_DEFINE_H_

#include "basic/basic_include.h"
#include "async/async_include.h"

#define RD_TIMER_NAMESPACE_BEGIN \
  RD_NAMESPACE_BEGIN             \
  namespace timer {

#define RD_TIMER_NAMESPACE_END \
  } /*namespace timer*/        \
                               \
  RD_NAMESPACE_END


#endif//RENDU_TIMER_TIMER_TIMER_DEFINE_H_
