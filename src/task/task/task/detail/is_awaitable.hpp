/*
* Created by boil on 2024/01/01.
*/

#ifndef RENDU_TASK_IS_AWAITABLE_HPP
#define RENDU_TASK_IS_AWAITABLE_HPP

#include "get_awaiter.hpp"

#include <type_traits>

TASK_NAMESPACE_BEGIN

template<typename T, typename = std::void_t<>>
struct IsAwaitable : std::false_type {};

template<typename T>
struct IsAwaitable<T, std::void_t<decltype(detail::get_awaiter(std::declval<T>()))>>
    : std::true_type {};

template<typename T>
constexpr bool IsAwaitable_v = IsAwaitable<T>::value;

TASK_NAMESPACE_END

#endif
