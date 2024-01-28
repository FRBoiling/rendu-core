/*
* Created by boil on 2024/01/01.
*/

#ifndef RENDU_ASYNC_DETAIL_GET_AWAITER_HPP
#define RENDU_ASYNC_DETAIL_GET_AWAITER_HPP

#include "detail/is_awaiter.hpp"
#include "detail/any.hpp"

ASYNC_NAMESPACE_BEGIN

namespace detail {
  template<typename T>
  auto get_awaiter_impl(T &&value, int) noexcept(noexcept(static_cast<T &&>(value).operator co_await()))
      -> decltype(static_cast<T &&>(value).operator co_await()) {
    return static_cast<T &&>(value).operator co_await();
  }

  template<typename T>
  auto get_awaiter_impl(T &&value, long) noexcept(noexcept(operator co_await(static_cast<T &&>(value))))
      -> decltype(operator co_await(static_cast<T &&>(value))) {
    return operator co_await(static_cast<T &&>(value));
  }

  template<
      typename T,
      std::enable_if_t<detail::IsAwaiter<T &&>::value, int> = 0>
  T &&get_awaiter_impl(T &&value, any) noexcept {
    return static_cast<T &&>(value);
  }

  template<typename T>
  auto get_awaiter(T &&value) noexcept(noexcept(detail::get_awaiter_impl(static_cast<T &&>(value), 123)))
      -> decltype(detail::get_awaiter_impl(static_cast<T &&>(value), 123)) {
    return detail::get_awaiter_impl(static_cast<T &&>(value), 123);
  }

  template<typename T>
  auto GetAwaiter(T &&value) {
    if constexpr (HasOperatorCoAwaitOperator<T>)
      return std::forward<T>(value).operator co_await();
    else if constexpr (HasAsAwaitableMethod<T>)
      return operator co_await(std::forward<T>(value).AsAwaitable());
    else
      return std::forward<T>(value);
  }


}// namespace detail

ASYNC_NAMESPACE_END

#endif
