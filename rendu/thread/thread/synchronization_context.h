/*
* Created by boil on 2024/2/5.
*/

#ifndef RENDU_THREAD_THREAD_SYNCHRONIZATION_CONTEXT_H_
#define RENDU_THREAD_THREAD_SYNCHRONIZATION_CONTEXT_H_

#include "thread.h"

THREAD_NAMESPACE_BEGIN

class SynchronizationContext {
public :
  static void SetSynchronizationContext(SynchronizationContext* sync_context);;
};

THREAD_NAMESPACE_END

#endif//RENDU_THREAD_THREAD_SYNCHRONIZATION_CONTEXT_H_
