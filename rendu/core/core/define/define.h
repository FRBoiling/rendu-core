/*
* Created by boil on 2023/2/11.
*/

#ifndef RENDU_CORE_DEFINE_DEFINE_H_
#define RENDU_CORE_DEFINE_DEFINE_H_

#include "common/define.h"

#if defined(__cpp_exceptions) && !defined(rendu_NOEXCEPTION)
#    define RD_CONSTEXPR
#    define RD_THROW throw
#    define RD_TRY try
#    define RD_CATCH catch(...)
#else
#    define RD_CONSTEXPR constexpr // use only with throwing functions (waiting for C++20)
#    define RD_THROW
#    define RD_TRY if(true)
#    define RD_CATCH if(false)
#endif

#ifndef RD_ID_TYPE
#    include <cstdint>
#    define RD_ID_TYPE std::uint32_t
#endif

#ifdef RD_USE_ATOMIC
#    include <atomic>
#    define RD_MAYBE_ATOMIC(Type) std::atomic<Type>
#else
#    define RD_MAYBE_ATOMIC(Type) Type
#endif

#ifndef RD_PACKED_PAGE
#    define RD_PACKED_PAGE 1024
#endif
#ifndef RD_SPARSE_PAGE
#    define RD_SPARSE_PAGE 4096
#endif

#   define RD_ETO_TYPE(Type) Type

#ifndef RD_ASSERT
#    include <cassert>
#    define RD_ASSERT(condition, msg) assert(condition)
#endif

#ifndef RD_ASSERT_CONSTEXPR
#    define RD_ASSERT_CONSTEXPR(condition, msg) RD_ASSERT(condition, msg)
#endif

#ifdef RD_STANDARD_CPP
#    define RD_NONSTD false
#else
#    define RD_NONSTD true
#    if defined __clang__ || defined __GNUC__
#        define RD_PRETTY_FUNCTION __PRETTY_FUNCTION__
#        define RD_PRETTY_FUNCTION_PREFIX '='
#        define RD_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#        define RD_PRETTY_FUNCTION __FUNCSIG__
#        define RD_PRETTY_FUNCTION_PREFIX '<'
#        define RD_PRETTY_FUNCTION_SUFFIX '>'
#    endif
#endif

#endif //RENDU_CORE_DEFINE_DEFINE_H_
