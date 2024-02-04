/*
* Created by boil on 2023/11/12.
*/

#ifndef RENDU_THREAD_DEFINE_H
#define RENDU_THREAD_DEFINE_H

#include "base/base.hpp"
#include "time/time.hpp"

#define THREAD_NAMESPACE_BEGIN \
  namespace rendu {            \
    namespace thread {

#define THREAD_NAMESPACE_END \
    } /*namespace thread*/   \
  } /*namespace rendu*/

using namespace rendu;
using namespace rendu::time;

#endif //RENDU_THREAD_DEFINE_H
