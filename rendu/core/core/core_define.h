/*
* Created by boil on 2023/11/15.
*/

#ifndef RENDU_CORE_DEFINE_H
#define RENDU_CORE_DEFINE_H

#include "log.hpp"
#include "net.h"
#include "task.hpp"

#define CORE_NAMESPACE_BEGIN \
  namespace rendu {            \
    namespace core {

#define CORE_NAMESPACE_END \
    } /*namespace common*/   \
  } /*namespace core*/

using namespace rendu::log;
using namespace rendu::task;
using namespace rendu::net;

#endif//RENDU_CORE_DEFINE_H
