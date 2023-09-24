/*
* Created by boil on 2023/9/28.
*/

#ifndef RENDU_THREAD_H
#define RENDU_THREAD_H

#include "define.h"
#include <thread>
#include <map>
#include "synchronization_context.h"
#include "base/scheduler.h"

RD_NAMESPACE_BEGIN
//    using Thread = std::thread;
    class Thread : public std::thread {
    public:
      // 使用 std::thread 的构造函数
      using std::thread::thread;

      explicit Thread(std::thread::id &id) : m_thread_id(id), m_current_context(nullptr) {
      }

      // 重写析构函数，如果线程可 joinable，则调用 join
      ~Thread() {
        if (joinable()) {
          join();
        }
      }

    public:

      SynchronizationContext *SetSynchronizationContext(SynchronizationContext *synchronizationContext);

      SynchronizationContext *GetSynchronizationContext();

    public:
      template<typename Func, typename Scheduler,typename Param>
      static Thread *Create(Func func, Scheduler pScheduler, Param param) {
        auto pThread = new Thread(func, pScheduler, param);
        m_threadsMap[pThread->get_id()] = pThread;
        return pThread;
      }

      static Thread *Create() {
        auto thread_id = std::this_thread::get_id();
        auto pThread = new Thread(thread_id);
        m_threadsMap[pThread->get_id()] = pThread;
        return pThread;
      }

      static Thread *GetCurrentThread();

      static void Sleep(int micro_seconds);

    private:
      static std::map<std::thread::id, Thread *> m_threadsMap;
      SynchronizationContext *m_current_context{};
      std::thread::id m_thread_id;
    };

    namespace Environment {
        int GetProcessorCount();
    }

RD_NAMESPACE_END

#endif //RENDU_THREAD_H
