#ifndef RENDU_TRAITS_SELECT_H_
#define RENDU_TRAITS_SELECT_H_

#include <type_traits>

namespace rendu::traits {
    template<bool Condition, template <typename...> typename Success>
    struct select {
        template <typename... T>
        using Args = Success<T...>;
    };

    template<template <typename...> typename Success>
    struct select<false, Success> {
        template <typename... T>
        using Args = std::void_t<T...>;
    };
}

#endif // RENDU_TRAITS_SELECT_H_