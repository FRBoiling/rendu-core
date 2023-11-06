/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_COMMON_AWAITABLE_TRAITS_H
#define RENDU_COMMON_AWAITABLE_TRAITS_H

#include "common/define.h"
#include "get_awaiter.h"

COMMON_NAMESPACE_BEGIN

    template<typename T, typename = void>
    struct awaitable_traits {
    };

    template<typename T>
    struct awaitable_traits<T, std::void_t<decltype(detail::get_awaiter(std::declval<T>()))>> {
      using awaiter_t = decltype(detail::get_awaiter(std::declval<T>()));

      using await_result_t = decltype(std::declval<awaiter_t>().await_resume());
    };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_AWAITABLE_TRAITS_H
