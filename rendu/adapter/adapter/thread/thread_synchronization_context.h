/*
* Created by boil on 2023/9/29.
*/

#ifndef RENDU_THREAD_SYNCHRONIZATION_CONTEXT_H
#define RENDU_THREAD_SYNCHRONIZATION_CONTEXT_H

#include "define.h"
#include "synchronization_context.h"
#include "concurrent/concurrent_queue.h"
#include "task/task.h"
#include <functional>

RD_NAMESPACE_BEGIN

    class ThreadSynchronizationContext : public SynchronizationContext {

    public:
      ThreadSynchronizationContext();

      ~ThreadSynchronizationContext();

    public:

      template<typename Func>
      void Post(Func func) {
        auto handler = func();
        m_queue.Enqueue(&handler);
      }

      void Update();
      // 线程同步队列,发送接收socket回调都放到该队列,由poll线程统一执行
    private:
      ConcurrentQueue<Task<> *> m_queue;
      Task<> *m_func;
    };

RD_NAMESPACE_END

#endif //RENDU_THREAD_SYNCHRONIZATION_CONTEXT_H
