/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_COMMON_GET_AWAITER_H
#define RENDU_COMMON_GET_AWAITER_H

#include "common/define.h"
#include "is_awaiter.h"
#include "any.h"

COMMON_NAMESPACE_BEGIN

    namespace detail {
        template<typename T>
        auto get_awaiter_impl(T &&value, int)
        noexcept(noexcept(static_cast<T &&>(value).operator

        co_await()))
        -> decltype(static_cast<T &&>(value).operator co_await()) {
        return static_cast<T&&>(value).operator co_await();
    }

    template<typename T>
    auto get_awaiter_impl(T &&value, long)
    noexcept(noexcept(operator

    co_await(static_cast<T&&>(value))))
    -> decltype(operator co_await(static_cast<T&&>(value))) {
    return operator co_await(static_cast<T&&>(value));
}

template<typename T, std::enable_if_t<detail::is_awaiter<T &&>::value, int> = 0>
T &&get_awaiter_impl(T &&value, detail::any) noexcept {
  return static_cast<T &&>(value);
}

template<typename T>
auto get_awaiter(T &&value)
noexcept(noexcept(detail::get_awaiter_impl(static_cast<T &&>(value), 123)))
-> decltype(detail::get_awaiter_impl(static_cast<T &&>(value), 123)) {
  return detail::get_awaiter_impl(static_cast<T &&>(value), 123);
}

}

COMMON_NAMESPACE_END
#endif //RENDU_COMMON_GET_AWAITER_H
