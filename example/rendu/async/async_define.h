/*
* Created by boil on 2024/01/01.
*/

#ifndef RENDU_ASYNC_DEFINE_H
#define RENDU_ASYNC_DEFINE_H

#include "config.hpp"
#include "log.hpp"
#include "utils/convert.h"
#include <coroutine>
#include <exception>
#include <functional>
#include <list>
#include <mutex>
#include <optional>

#define ASYNC_NAMESPACE_BEGIN \
  namespace rendu {            \
    namespace async {

#define ASYNC_NAMESPACE_END \
    } /*namespace async*/   \
  } /*namespace rendu*/


#endif//RENDU_ASYNC_DEFINE_H
