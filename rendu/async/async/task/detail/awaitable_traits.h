/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_AWAITABLE_TRAITS_H
#define RENDU_AWAITABLE_TRAITS_H

#include "async_define.h"
#include "get_awaiter.h"

ASYNC_NAMESPACE_BEGIN

    template<typename T, typename = void>
    struct awaitable_traits {
    };

    template<typename T>
    struct awaitable_traits<T, std::void_t<decltype(detail::get_awaiter(std::declval<T>()))>> {
      using awaiter_t = decltype(detail::get_awaiter(std::declval<T>()));

      using await_result_t = decltype(std::declval<awaiter_t>().await_resume());
    };

ASYNC_NAMESPACE_END

#endif //RENDU_AWAITABLE_TRAITS_H
