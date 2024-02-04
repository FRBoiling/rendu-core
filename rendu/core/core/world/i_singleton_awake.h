/*
* Created by boil on 2024/2/5.
*/

#ifndef RENDU_CORE_CORE_WORLD_I_SINGLETON_AWAKE_H_
#define RENDU_CORE_CORE_WORLD_I_SINGLETON_AWAKE_H_

#include "core_define.h"


CORE_NAMESPACE_BEGIN

template<typename... Args>
class ISingletonAwake
{
  virtual void Awake(Args ...args) = 0;
};

template<>
class ISingletonAwake<void>
{
  virtual void Awake() = 0;
};

CORE_NAMESPACE_END

#endif//RENDU_CORE_CORE_WORLD_I_SINGLETON_AWAKE_H_
