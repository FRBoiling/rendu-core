/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_RUNTIME_DEFINE_H_
#define RENDU_RUNTIME_DEFINE_H_

#include "base/base.hpp"

#define RUNTIME_NAMESPACE_BEGIN \
  namespace rendu {         \
    namespace runtime {

#define RUNTIME_NAMESPACE_END \
  } /*namespace runtime*/     \
  } /*namespace rendu*/

using namespace rendu;

#endif//RENDU_RUNTIME_DEFINE_H_
