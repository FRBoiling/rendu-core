/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_ANY_H
#define RENDU_ANY_H


#include "define.h"

RD_NAMESPACE_BEGIN
    namespace detail {
        // Helper type that can be cast-to from any type.
        struct any {
          template<typename T>
          any(T &&) noexcept {}
        };

        struct void_value {};
    }
RD_NAMESPACE_END


#endif //RENDU_ANY_H
