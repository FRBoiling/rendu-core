/*
* Created by boil on 2024/1/20.
*/

#ifndef RENDU_AWAITABLE_TRANSFORM_HPP
#define RENDU_AWAITABLE_TRANSFORM_HPP

#include "awaitable_traits.hpp"
#include "async_define.h"

ASYNC_NAMESPACE_BEGIN

namespace detail {

  template<typename Awaitable>
  inline auto AwaitTransform(Awaitable &&awaitable) {
    if constexpr (detail::HasAsAwaitableMethod<Awaitable>) {
      return detail::GetAwaiter( std::forward<Awaitable>(awaitable).AsAwaitable());
    } else {
//      using AwaiterType = decltype(detail::GetAwaiter(std::forward<Awaitable>(awaitable)));
//      return ViaAsyncAwaiter<std::decay_t<AwaiterType>>(std::forward<Awaitable>(awaitable));
    }
  }


}// namespace detail

ASYNC_NAMESPACE_END

#endif//RENDU_AWAITABLE_TRANSFORM_HPP
