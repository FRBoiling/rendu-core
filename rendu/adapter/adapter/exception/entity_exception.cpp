/*
* Created by boil on 2023/9/29.
*/

#include "entity_exception.h"

RD_NAMESPACE_BEGIN

    EntityException::EntityException(const std::string &msg) {

    }

    const char *EntityException::what() const noexcept {
      return "EntityException";
    }

    EntityException::~EntityException() = default;

RD_NAMESPACE_END
