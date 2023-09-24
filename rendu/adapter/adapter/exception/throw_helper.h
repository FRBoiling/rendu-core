/*
* Created by boil on 2023/9/23.
*/

#ifndef RENDU_THROW_HELPER_H
#define RENDU_THROW_HELPER_H

#include "define.h"
#include "argument_exception.h"
#include "entity_exception.h"

RD_NAMESPACE_BEGIN

    class ThrowHelper {

    public:
      static ArgumentException ArgumentException(const char *message);
      static EntityException EntityException(const char *message);

    };

RD_NAMESPACE_END
#endif //RENDU_THROW_HELPER_H
