/*
* Created by boil on 2024/01/01.
*/

#ifndef RENDU_ASYNC_DEFINE_H
#define RENDU_ASYNC_DEFINE_H

#include "log.h"
#include "utils/convert.h"
#include "config.hpp"
#include <functional>
#include <mutex>
#include <list>
#include <optional>
#include <coroutine>
#include <exception>

#define ASYNC_NAMESPACE_BEGIN \
  namespace rendu {            \
    namespace async {

#define ASYNC_NAMESPACE_END \
    } /*namespace async*/   \
  } /*namespace rendu*/


#endif//RENDU_ASYNC_DEFINE_H
