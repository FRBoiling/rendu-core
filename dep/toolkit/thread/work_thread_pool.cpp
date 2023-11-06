/*
* Created by boil on 2023/10/26.
*/

#include "work_thread_pool.h"
#include "utils/instance_imp.h"
#include "thread_pool.h"

RD_NAMESPACE_BEGIN

  static size_t s_pool_size = 0;
  static bool s_enable_cpu_affinity = true;

  INSTANCE_IMP(WorkThreadPool)

  WorkThreadPool::WorkThreadPool() {
    //最低优先级
    addPoller("work poller", s_pool_size, ThreadPool::PRIORITY_LOWEST, false, s_enable_cpu_affinity);
  }

  EventLoop::Ptr WorkThreadPool::GetFirstPoller() {
    return std::static_pointer_cast<EventLoop>(_threads.front());
  }

  EventLoop::Ptr WorkThreadPool::GetPoller() {
    return std::static_pointer_cast<EventLoop>(GetExecutor());
  }

  void WorkThreadPool::SetPoolSize(size_t size) {
    s_pool_size = size;
  }

  void WorkThreadPool::EnableCpuAffinity(bool enable) {
    s_enable_cpu_affinity = enable;
  }

RD_NAMESPACE_END