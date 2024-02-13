/*
* Created by boil on 2024/3/21.
*/

#ifndef RENDU_THREAD_THREAD_EXECUTION_CONTEXT_H_
#define RENDU_THREAD_THREAD_EXECUTION_CONTEXT_H_

#include "thread_define.h"

THREAD_NAMESPACE_BEGIN

class ExecutionContext {

public:
  static ExecutionContext* Capture() {
    ExecutionContext *executionContext = Thread::CurrentThread._executionContext;
    if (executionContext == nullptr)
      return ExecutionContext::Default;
    else if (executionContext->m_isFlowSuppressed)
      return nullptr;
  }
};

THREAD_NAMESPACE_END

#endif//RENDU_THREAD_THREAD_EXECUTION_CONTEXT_H_
