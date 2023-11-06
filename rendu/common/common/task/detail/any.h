/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_COMMON_ANY_H
#define RENDU_COMMON_ANY_H


#include "common/common_define.h"

COMMON_NAMESPACE_BEGIN
    namespace detail {
        // Helper type that can be cast-to from any type.
        struct any {
          template<typename T>
          any(T &&) noexcept {}
        };

        struct void_value {};
    }
COMMON_NAMESPACE_END


#endif //RENDU_COMMON_ANY_H
