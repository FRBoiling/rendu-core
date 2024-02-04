/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_INVOKE_HANDLER_H
#define RENDU_INVOKE_HANDLER_H

#include "core_define.h"
#include <typeinfo>

CORE_NAMESPACE_BEGIN

    template<typename A, typename T>
    class AInvokeHandler {
    public:
      std::type_info Type;
    public:
      virtual T Handle(A args){}
    };

CORE_NAMESPACE_END

#endif //RENDU_INVOKE_HANDLER_H
