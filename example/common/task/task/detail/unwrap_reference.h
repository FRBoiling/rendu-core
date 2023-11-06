/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_UNWRAP_REFERENCE_H
#define RENDU_UNWRAP_REFERENCE_H


#include "define.h"
#include <functional>

RD_NAMESPACE_BEGIN
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

RD_NAMESPACE_END
#endif //RENDU_UNWRAP_REFERENCE_H
