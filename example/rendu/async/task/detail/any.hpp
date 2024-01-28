/*
* Created by boil on 2024/01/01.
*/

#ifndef RENDU_ASYNC_DETAIL_ANY_HPP
#define RENDU_ASYNC_DETAIL_ANY_HPP

#include "async_define.h"

ASYNC_NAMESPACE_BEGIN

namespace detail {
  // Helper type that can be cast-to from any type.
  struct any {
    template<typename T>
    any(T &&) noexcept {}
  };
}// namespace detail

ASYNC_NAMESPACE_END

#endif
