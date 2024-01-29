/*
* Created by boil on 2023/11/12.
*/

#ifndef RENDU_ASYNC_DEFINE_H
#define RENDU_ASYNC_DEFINE_H

#include "log/log.hpp"
#include "time/time.hpp"

#define ASYNC_NAMESPACE_BEGIN \
  namespace rendu {            \
    namespace async {

#define ASYNC_NAMESPACE_END \
    } /*namespace async*/   \
  } /*namespace rendu*/

using namespace rendu::log;
using namespace rendu::time;

#endif //RENDU_ASYNC_DEFINE_H
