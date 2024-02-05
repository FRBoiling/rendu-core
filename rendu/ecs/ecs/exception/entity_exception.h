/*
* Created by boil on 2024/2/5.
*/

#ifndef RENDU_ECS_ECS_EXCEPTION_ENTITY_EXCEPTION_H_
#define RENDU_ECS_ECS_EXCEPTION_ENTITY_EXCEPTION_H_

#include "ecs_define.h"

ECS_NAMESPACE_BEGIN

class EntityException : public Exception {
public:
  template<typename... Args>
  EntityException(string_view_t fmt, Args &&...args): Exception(fmt,args...) {
  }
};

ECS_NAMESPACE_END

#endif//RENDU_ECS_ECS_EXCEPTION_ENTITY_EXCEPTION_H_
