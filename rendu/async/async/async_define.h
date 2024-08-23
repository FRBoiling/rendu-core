/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_ASYNC_ASYNC_ASYNC_DEFINE_H_
#define RENDU_ASYNC_ASYNC_ASYNC_DEFINE_H_

#include "basic/basic_include.h"

#define RD_ASYNC_NAMESPACE_BEGIN \
  RD_NAMESPACE_BEGIN             \
  namespace async {

#define RD_ASYNC_NAMESPACE_END \
  } /*namespace async*/        \
                               \
  RD_NAMESPACE_END

#endif//RENDU_ASYNC_ASYNC_ASYNC_DEFINE_H_
