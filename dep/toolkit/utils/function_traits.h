/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_FUNCTION_TRAITS_H
#define RENDU_FUNCTION_TRAITS_H

#include <tuple>
#include <functional>

#include "define.h"

RD_NAMESPACE_BEGIN

  template<typename T>
  struct FunctionTraits;

//普通函数
  template<typename Ret, typename... Args>
  struct FunctionTraits<Ret(Args...)> {
  public:
    enum {
      arity = sizeof...(Args)
    };

    typedef Ret function_type(Args...);

    typedef Ret return_type;
    using stl_function_type = std::function<function_type>;

    typedef Ret(*pointer)(Args...);

    template<size_t I>
    struct args {
      static_assert(I < arity, "index is out of range, index must less than sizeof Args");
      using type = typename std::tuple_element<I, std::tuple<Args...> >::type;
    };
  };

//函数指针
  template<typename Ret, typename... Args>
  struct FunctionTraits<Ret(*)(Args...)> : FunctionTraits<Ret(Args...)> {
  };

//std::function
  template<typename Ret, typename... Args>
  struct FunctionTraits<std::function<Ret(Args...)>> : FunctionTraits<Ret(Args...)> {
  };

//member function
#define FUNCTION_TRAITS(...) \
    template <typename ReturnType, typename ClassType, typename... Args>\
    struct FunctionTraits<ReturnType(ClassType::*)(Args...) __VA_ARGS__> : FunctionTraits<ReturnType(Args...)>{}; \

  FUNCTION_TRAITS()
  FUNCTION_TRAITS(const)
  FUNCTION_TRAITS(volatile)
  FUNCTION_TRAITS(const volatile)

//函数对象
  template<typename Callable>
  struct FunctionTraits : FunctionTraits<decltype(&Callable::operator())> {
  };

RD_NAMESPACE_END

#endif //RENDU_FUNCTION_TRAITS_H
