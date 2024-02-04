/*
* Created by boil on 2024/2/5.
*/

#ifndef RENDU_CORE_CORE_WORLD_FIBER_I_SCHEDULER_H_
#define RENDU_CORE_CORE_WORLD_FIBER_I_SCHEDULER_H_

#include "core_define.h"

CORE_NAMESPACE_BEGIN

class IScheduler {
public:
  virtual void Add(INT32 fiberId) = 0;
};

CORE_NAMESPACE_END

#endif//RENDU_CORE_CORE_WORLD_FIBER_I_SCHEDULER_H_
