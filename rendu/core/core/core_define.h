/*
* Created by boil on 2023/11/15.
*/

#ifndef RENDU_CORE_DEFINE_H
#define RENDU_CORE_DEFINE_H

#include "async.hpp"
#include "common.h"
#include "log.hpp"
#include "net.h"

#define CORE_NAMESPACE_BEGIN \
  namespace rendu {            \
    namespace core {

#define CORE_NAMESPACE_END \
    } /*namespace common*/   \
  } /*namespace core*/

using namespace rendu;
using namespace rendu::log;
using namespace rendu::async;
using namespace rendu::net;

#endif//RENDU_CORE_DEFINE_H
