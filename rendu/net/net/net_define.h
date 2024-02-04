/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_NET_DEFINE_H_
#define RENDU_NET_DEFINE_H_

#include "log/log.hpp"
#include "time/time.hpp"
#include "task/task.hpp"

#define NET_NAMESPACE_BEGIN \
  namespace rendu {            \
    namespace net {

#define NET_NAMESPACE_END \
    } /*namespace net*/   \
  } /*namespace rendu*/

using namespace rendu::log;
using namespace rendu::time;
using namespace rendu::task;

#endif//RENDU_NET_DEFINE_H_
