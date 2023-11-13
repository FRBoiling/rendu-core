/*
* Created by boil on 2023/9/29.
*/

#ifndef RENDU_COMMON_SYNCHRONIZATION_CONTEXT_H
#define RENDU_COMMON_SYNCHRONIZATION_CONTEXT_H

#include "common_define.h"

COMMON_NAMESPACE_BEGIN

    class SynchronizationContext {
    public:
      static void SetSynchronizationContext(SynchronizationContext *pContext);
      static SynchronizationContext* GetCurrentSynchronizationContext();
    };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_SYNCHRONIZATION_CONTEXT_H
