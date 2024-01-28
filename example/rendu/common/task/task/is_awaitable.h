/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_IS_AWAITABLE_H
#define RENDU_IS_AWAITABLE_H

#include "define.h"
#include "detail/get_awaiter.h"

RD_NAMESPACE_BEGIN

template<typename T, typename = std::void_t<>>
struct is_awaitable : std::false_type {};

template<typename T>
struct is_awaitable<T, std::void_t<decltype(detail::get_awaiter(std::declval<T>()))>>
: std::true_type
{};

template<typename T>
constexpr bool is_awaitable_v = is_awaitable<T>::value;

RD_NAMESPACE_END

#endif //RENDU_IS_AWAITABLE_H
