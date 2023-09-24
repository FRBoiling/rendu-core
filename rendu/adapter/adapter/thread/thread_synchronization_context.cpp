/*
* Created by boil on 2023/9/29.
*/

#include "thread_synchronization_context.h"
#include "logger/log.h"

RD_NAMESPACE_BEGIN

    ThreadSynchronizationContext::ThreadSynchronizationContext() {

    }

    ThreadSynchronizationContext::~ThreadSynchronizationContext() {

    }

    void ThreadSynchronizationContext::Update() {
      while (true) {
        if (!m_queue.TryDequeue(m_func)) {
          return;
        }
        try {
//          (m_func)();
        }
        catch (std::exception &e) {
          RD_CRITICAL(e.what());
        }
      }
    }


RD_NAMESPACE_END
