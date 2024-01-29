/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_LOG_DEFINE_H_
#define RENDU_LOG_DEFINE_H_

#include "base.hpp"

#define LOG_NAMESPACE_BEGIN \
  namespace rendu {         \
    namespace log {

#define LOG_NAMESPACE_END \
  } /*namespace log*/     \
  } /*namespace rendu*/

using namespace rendu;

#endif//RENDU_LOG_DEFINE_H_
