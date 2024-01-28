/*
* Created by boil on 2024/01/01.
*/

#ifndef RENDU_ASYNC_BROKEN_PROMISE_HPP
#define RENDU_ASYNC_BROKEN_PROMISE_HPP

#include "async_define.h"
#include <stdexcept>

ASYNC_NAMESPACE_BEGIN

namespace detail {

  class BrokenPromise : public std::logic_error {
  public:
    BrokenPromise()
        : std::logic_error("broken promise") {}
  };

}// namespace detail

ASYNC_NAMESPACE_END

#endif
