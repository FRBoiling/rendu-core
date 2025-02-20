/*
* Created by boil on 2024/01/01.
*/

#ifndef RENDU_TASK_BROKEN_PROMISE_HPP
#define RENDU_TASK_BROKEN_PROMISE_HPP

#include "task_define.h"
#include <stdexcept>

TASK_NAMESPACE_BEGIN

namespace detail {

  class BrokenPromise : public std::logic_error {
  public:
    BrokenPromise()
        : std::logic_error("broken promise") {}
  };

}// namespace detail

TASK_NAMESPACE_END

#endif
