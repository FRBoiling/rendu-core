/*
* Created by boil on 2023/9/29.
*/

#include "synchronization_context.h"
#include "thread.h"

RD_NAMESPACE_BEGIN

    void SynchronizationContext::SetSynchronizationContext(SynchronizationContext *pContext) {
      Thread::GetCurrentThread()->SetSynchronizationContext(pContext);
    }

    SynchronizationContext *SynchronizationContext::GetCurrentSynchronizationContext() {
      return Thread::GetCurrentThread()->GetSynchronizationContext();
    }

RD_NAMESPACE_END


