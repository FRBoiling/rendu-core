/*
* Created by boil on 2023/9/29.
*/

#ifndef RENDU_ASYNC_SYNCHRONIZATION_CONTEXT_H
#define RENDU_ASYNC_SYNCHRONIZATION_CONTEXT_H

#include "async_define.h"

ASYNC_NAMESPACE_BEGIN

    class SynchronizationContext {
    public:
      static void SetSynchronizationContext(SynchronizationContext *pContext);
      static SynchronizationContext* GetCurrentSynchronizationContext();
    };

ASYNC_NAMESPACE_END

#endif //RENDU_ASYNC_SYNCHRONIZATION_CONTEXT_H
