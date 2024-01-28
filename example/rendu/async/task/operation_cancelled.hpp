/*
* Created by boil on 2024/01/01.
*/

#ifndef RENDU_ASYNC_OPERATION_CANCELLED_HPP
#define RENDU_ASYNC_OPERATION_CANCELLED_HPP

#include "async_define.h"
#include <exception>

ASYNC_NAMESPACE_BEGIN

class operation_cancelled : public std::exception {
public:
  operation_cancelled() noexcept
      : std::exception() {}

  const char *what() const noexcept override { return "operation cancelled"; }
};

ASYNC_NAMESPACE_END

#endif
