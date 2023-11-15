/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_FIBER_INIT_H
#define RENDU_FIBER_INIT_H

#include "core_define.h"
#include "fiber.h"

RD_NAMESPACE_BEGIN

    class FiberInit {
    public:
      explicit FiberInit(Fiber &fiber) : m_fiber(&fiber) {}
    public:
      Fiber *m_fiber;
    };

RD_NAMESPACE_END

#endif //RENDU_FIBER_INIT_H
