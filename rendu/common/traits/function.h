#ifndef RENDU_TRAITS_FUNCTIONINFO_H_
#define RENDU_TRAITS_FUNCTIONINFO_H_


#include "identity.h"
#include "arguments.h"

namespace rendu::traits
{
    template <typename Fn>
    struct function : function<decltype(&Fn::operator())> {};

    template <typename Ret, typename T, typename... Args>
    struct function<Ret(T::*)(Args...) const> : identity<Ret(T::*)(Args...) const>, Arguments<Args...> {
        using Return = Ret;
    };

    template <typename Ret, typename T, typename... Args>
    struct function<Ret(T::*)(Args...)> : identity<Ret(T::*)(Args...)>, Arguments<Args...> {
        using Return = Ret;
    };

    template <typename Ret, typename... Args>
    struct function<Ret(*)(Args...)> : identity<Ret(*)(Args...)>, Arguments<Args...> {
        using Return = Ret;
    };
}

#endif // RENDU_TRAITS_FUNCTIONINFO_H_