/*
* Created by boil on 2023/10/26.
*/

#include "event_loop_pool.h"
#include "utils/instance_imp.h"
#include "thread/thread_pool.h"
#include "notice_helper.h"

RD_NAMESPACE_BEGIN

  static size_t s_pool_size = 0;
  static bool s_enable_cpu_affinity = true;

  INSTANCE_IMP(EventLoopPool)

  EventLoop::Ptr EventLoopPool::getFirstPoller() {
    return static_pointer_cast<EventLoop>(_threads.front());
  }

  EventLoop::Ptr EventLoopPool::getPoller(bool prefer_current_thread) {
    auto poller = EventLoop::getCurrentPoller();
    if (prefer_current_thread && _prefer_current_thread && poller) {
      return poller;
    }
    return static_pointer_cast<EventLoop>(GetExecutor());
  }

  void EventLoopPool::preferCurrentThread(bool flag) {
    _prefer_current_thread = flag;
  }

  const std::string EventLoopPool::kOnStarted = "kBroadcastEventPollerPoolStarted";

  EventLoopPool::EventLoopPool() {
    auto size = addPoller("event poller", s_pool_size, ThreadPool::PRIORITY_HIGHEST, true, s_enable_cpu_affinity);
    NOTICE_EMIT(EventPollerPoolOnStartedArgs, kOnStarted, *this, size);
    LOG_INFO << "EventLoop created Size: " << size;
  }

  void EventLoopPool::setPoolSize(size_t size) {
    s_pool_size = size;
  }

  void EventLoopPool::enableCpuAffinity(bool enable) {
    s_enable_cpu_affinity = enable;
  }

RD_NAMESPACE_END