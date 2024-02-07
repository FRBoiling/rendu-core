/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_NET_DEFINE_H_
#define RENDU_NET_DEFINE_H_

#include "log/log.hpp"
#include "runtime/runtime.hpp"
#include "io/io.hpp"

#define NET_NAMESPACE_BEGIN \
  namespace rendu {            \
    namespace net {

#define NET_NAMESPACE_END \
    } /*namespace net*/   \
  } /*namespace rendu*/

using namespace rendu::log;
using namespace rendu::io;
using namespace rendu::runtime;

#endif//RENDU_NET_DEFINE_H_
