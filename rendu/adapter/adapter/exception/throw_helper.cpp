/*
* Created by boil on 2023/9/23.
*/

#include "throw_helper.h"

RD_NAMESPACE_BEGIN

    ArgumentException ThrowHelper::ArgumentException(const char *message) {
      throw ArgumentException(message);
    }

    EntityException ThrowHelper::EntityException(const char *message) {
      return EntityException(message);
    }

RD_NAMESPACE_END