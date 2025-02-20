/*
* Created by boil on 2023/11/12.
*/

#ifndef RENDU_TASK_DEFINE_H
#define RENDU_TASK_DEFINE_H

#include "log/log.hpp"
#include "time/time.hpp"
#include "thread/thread.hpp"

#define TASK_NAMESPACE_BEGIN \
  namespace rendu {            \
    namespace task {

#define TASK_NAMESPACE_END \
    } /*namespace task*/   \
  } /*namespace rendu*/

using namespace rendu::log;
using namespace rendu::time;
using namespace rendu::thread;

#endif //RENDU_TASK_DEFINE_H
