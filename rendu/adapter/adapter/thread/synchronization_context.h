/*
* Created by boil on 2023/9/29.
*/

#ifndef RENDU_SYNCHRONIZATION_CONTEXT_H
#define RENDU_SYNCHRONIZATION_CONTEXT_H

#include "define.h"

RD_NAMESPACE_BEGIN

    class SynchronizationContext {
    public:
      static void SetSynchronizationContext(SynchronizationContext *pContext);
      static SynchronizationContext* GetCurrentSynchronizationContext();
    };

RD_NAMESPACE_END

#endif //RENDU_SYNCHRONIZATION_CONTEXT_H
