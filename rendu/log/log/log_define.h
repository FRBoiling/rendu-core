/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_LOG_DEFINE_H_
#define RENDU_LOG_DEFINE_H_

#include "base/base.hpp"
#include "time/time.hpp"

#define LOG_NAMESPACE_BEGIN \
  namespace rendu {         \
    namespace log {

#define LOG_NAMESPACE_END \
  } /*namespace log*/     \
  } /*namespace rendu*/

using namespace rendu;
using namespace rendu::time;

#endif//RENDU_LOG_DEFINE_H_
