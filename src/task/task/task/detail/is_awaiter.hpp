/*
* Created by boil on 2024/01/01.
*/

#ifndef RENDU_TASK_DETAIL_IS_AWAITER_HPP
#define RENDU_TASK_DETAIL_IS_AWAITER_HPP

#include "task_define.h"
#include <coroutine>
#include <type_traits>

TASK_NAMESPACE_BEGIN

namespace detail {
  template<typename T>
  struct IsCoroutineHandle
      : std::false_type {};

  template<typename PROMISE>
  struct IsCoroutineHandle<std::coroutine_handle<PROMISE>>
      : std::true_type {};

  // NOTE: We're accepting a return value of coroutine_handle<P> here
  // which is an extension supported by Clang which is not yet part of
  // the C++ coroutines TS.
  template<typename T>
  struct IsValidAwaitSuspendReturnValue : std::disjunction<
                                                   std::is_void<T>,
                                                   std::is_same<T, bool>,
                                                   IsCoroutineHandle<T>> {};

  template<typename T, typename = std::void_t<>>
  struct IsAwaiter : std::false_type {};

  // NOTE: We're testing whether await_suspend() will be callable using an
  // arbitrary coroutine_handle here by checking if it supports being passed
  // a coroutine_handle<void>. This may result in a false-result for some
  // types which are only awaitable within a certain context.
  template<typename T>
  struct IsAwaiter<T, std::void_t<
                           decltype(std::declval<T>().await_ready()),
                           decltype(std::declval<T>().await_suspend(std::declval<std::coroutine_handle<>>())),
                           decltype(std::declval<T>().await_resume())>> : std::conjunction<std::is_constructible<bool, decltype(std::declval<T>().await_ready())>,
                                                                                           detail::IsValidAwaitSuspendReturnValue<
                                                                                               decltype(std::declval<T>().await_suspend(std::declval<std::coroutine_handle<>>()))>> {};


  template<class T>
  concept HasOperatorCoAwaitOperator = requires(T &&awaitable) {
    std::forward<T>(awaitable).operator co_await();
  };

  template<class T>
  concept HasAsAwaitableMethod = requires(T &&awaitable) {
    std::forward<T>(awaitable).AsAwaitable();
  };

}// namespace detail

TASK_NAMESPACE_END

#endif
