/*
* Created by boil on 2023/9/29.
*/

#include "synchronization_context.h"
#include "thread.h"

ASYNC_NAMESPACE_BEGIN

    void SynchronizationContext::SetSynchronizationContext(SynchronizationContext *pContext) {
      Thread::GetCurrentThread()->SetSynchronizationContext(pContext);
    }

    SynchronizationContext *SynchronizationContext::GetCurrentSynchronizationContext() {
      return Thread::GetCurrentThread()->GetSynchronizationContext();
    }

ASYNC_NAMESPACE_END


