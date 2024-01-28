/*
* Created by boil on 2024/01/01.
*/


#ifndef RENDU_ASYNC_AWAITABLE_TRAITS_HPP
#define RENDU_ASYNC_AWAITABLE_TRAITS_HPP

#include "get_awaiter.hpp"

#include <type_traits>

ASYNC_NAMESPACE_BEGIN

namespace detail {

  template<typename T, typename = void>
  struct AwaitableTraits {};

  template<typename T>
  struct AwaitableTraits<T, std::void_t<decltype(detail::get_awaiter(std::declval<T>()))>> {
    using awaiter_t = decltype(detail::get_awaiter(std::declval<T>()));

    using await_result_t = decltype(std::declval<awaiter_t>().await_resume());
  };

}// namespace detail
ASYNC_NAMESPACE_END

#endif
