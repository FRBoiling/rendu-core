/*
* Created by boil on 2023/11/12.
*/

#ifndef RENDU_ASYNC_DEFINE_H
#define RENDU_ASYNC_DEFINE_H

#include "common.h"
#include "log.h"

#define ASYNC_NAMESPACE_BEGIN \
  namespace rendu {            \
    namespace async {

#define ASYNC_NAMESPACE_END \
    } /*namespace async*/   \
  } /*namespace rendu*/

using namespace rendu::common;
using namespace rendu::log;

#endif //RENDU_ASYNC_DEFINE_H
