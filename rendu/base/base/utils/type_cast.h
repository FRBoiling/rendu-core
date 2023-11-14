/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_BASE_TYPE_CAST_H
#define RENDU_BASE_TYPE_CAST_H

#include "base_define.h"

RD_NAMESPACE_BEGIN

  namespace TypeCast {
    inline void memZero(void *p, size_t n) {
      memset(p, 0, n);
    }

    template<typename To, typename From>
    inline To implicit_cast(From const &f) {
      return f;
    }

    template<typename To, typename From>
    // use like this: down_cast<T*>(foo);
    inline To down_cast(From *f)                     // so we only accept pointers
    {
      // Ensures that To is a sub-type of From *.  This test is here only
      // for compile-time type checking, and has no overhead in an
      // optimized build at run-time, as it will be optimized away
      // completely.
      if (false) {
        implicit_cast<From *, To>(0);
      }

#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
      assert(f == NULL || dynamic_cast<To>(f) != NULL);  // RTTI: debug mode only!
#endif
      return static_cast<To>(f);
    }
  }
  
RD_NAMESPACE_END

#endif //RENDU_BASE_TYPE_CAST_H
