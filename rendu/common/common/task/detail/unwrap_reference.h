/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_COMMON_UNWRAP_REFERENCE_H
#define RENDU_COMMON_UNWRAP_REFERENCE_H


#include "common/common_define.h"
#include <functional>

COMMON_NAMESPACE_BEGIN
    namespace detail {
        template<typename T>
        struct unwrap_reference {
          using type = T;
        };

        template<typename T>
        struct unwrap_reference<std::reference_wrapper<T>> {
          using type = T;
        };

        template<typename T>
        using unwrap_reference_t = typename unwrap_reference<T>::type;
    }

COMMON_NAMESPACE_END
#endif //RENDU_COMMON_UNWRAP_REFERENCE_H
