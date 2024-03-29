/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_IS_AWAITER_H
#define RENDU_IS_AWAITER_H

#include "define.h"
#include <coroutine>

RD_NAMESPACE_BEGIN

    namespace detail {
        template<typename T>
        struct is_coroutine_handle
            : std::false_type {
        };

        template<typename PROMISE>
        struct is_coroutine_handle<std::coroutine_handle<PROMISE>>
            : std::true_type {
        };

// NOTE: We're accepting a return value of coroutine_handle<P> here
// which is an extension supported by Clang which is not yet part of
// the C++ coroutines TS.
        template<typename T>
        struct is_valid_await_suspend_return_value : std::disjunction<
            std::is_void<T>,
            std::is_same<T, bool>,
            is_coroutine_handle<T>> {
        };

        template<typename T, typename = std::void_t<>>
        struct is_awaiter : std::false_type {
        };

// NOTE: We're testing whether await_suspend() will be callable using an
// arbitrary coroutine_handle here by checking if it supports being passed
// a coroutine_handle<void>. This may result in a false-result for some
// types which are only awaitable within a certain context.
        template<typename T>
        struct is_awaiter<T, std::void_t<
            decltype(std::declval<T>().await_ready()),
            decltype(std::declval<T>().await_suspend(std::declval<std::coroutine_handle<>>())),
            decltype(std::declval<T>().await_resume())>> :
            std::conjunction<
                std::is_constructible<bool, decltype(std::declval<T>().await_ready())>,
                detail::is_valid_await_suspend_return_value<
                    decltype(std::declval<T>().await_suspend(std::declval<std::coroutine_handle<>>()))>> {
        };
    }

RD_NAMESPACE_END

#endif //RENDU_IS_AWAITER_H
