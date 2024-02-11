/*
* Created by boil on 2023/11/15.
*/

#ifndef RENDU_CORE_DEFINE_H
#define RENDU_CORE_DEFINE_H

#include "log/log.hpp"
#include "net/net.hpp"
#include "task/task.hpp"
#include "io/io.hpp"
#include "ecs/ecs.hpp"
#include "serialize/serialize.hpp"

#define CORE_NAMESPACE_BEGIN \
  namespace rendu {            \
    namespace core {

#define CORE_NAMESPACE_END \
    } /*namespace common*/   \
  } /*namespace core*/

using namespace rendu::log;
using namespace rendu::task;
using namespace rendu::io;
using namespace rendu::net;
using namespace rendu::ecs;
using namespace rendu::serialize;

#endif//RENDU_CORE_DEFINE_H
