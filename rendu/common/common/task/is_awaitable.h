/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_COMMON_IS_AWAITABLE_H
#define RENDU_COMMON_IS_AWAITABLE_H

#include "common/common_define.h"
#include "common/task/detail/get_awaiter.h"

COMMON_NAMESPACE_BEGIN

template<typename T, typename = std::void_t<>>
struct is_awaitable : std::false_type {};

template<typename T>
struct is_awaitable<T, std::void_t<decltype(detail::get_awaiter(std::declval<T>()))>>
: std::true_type
{};

template<typename T>
constexpr bool is_awaitable_v = is_awaitable<T>::value;

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_IS_AWAITABLE_H
