/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_NET_DEFINE_H_
#define RENDU_NET_DEFINE_H_

#include "common.h"
#include "log.h"
#include "async.h"

#define NET_NAMESPACE_BEGIN \
  namespace rendu {            \
    namespace net {

#define NET_NAMESPACE_END \
    } /*namespace net*/   \
  } /*namespace rendu*/

using namespace rendu::common;
using namespace rendu::log;
using namespace rendu::async;

#endif//RENDU_NET_DEFINE_H_
