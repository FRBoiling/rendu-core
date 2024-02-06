/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_IO_DEFINE_H_
#define RENDU_IO_DEFINE_H_

#include "base/base.hpp"
#include "task/task.hpp"

#define IO_NAMESPACE_BEGIN \
  namespace rendu {        \
    namespace io {

#define IO_NAMESPACE_END \
  } /*namespace io*/     \
  } /*namespace rendu*/

using namespace rendu;
using namespace rendu::task;

#endif//RENDU_IO_DEFINE_H_
