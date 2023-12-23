/*
* Created by boil on 2023/9/29.
*/

#include "thread_synchronization_context.h"

ASYNC_NAMESPACE_BEGIN
using namespace common;

    ThreadSynchronizationContext::ThreadSynchronizationContext() {

    }

    ThreadSynchronizationContext::~ThreadSynchronizationContext() {

    }

    void ThreadSynchronizationContext::Update() {
      while (true) {
        if (!m_queue.TryDequeue(m_func) || !m_func) {
          return;
        }
        try {

        }
        catch (std::exception &e) {
         RD_CRITICAL(e.what());
        }
      }
    }


ASYNC_NAMESPACE_END
