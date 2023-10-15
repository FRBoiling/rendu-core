/*
* Created by boil on 2023/10/26.
*/

#include "event_poller_pool.h"
#include "utils/instance_imp.h"
#include "thread/thread_pool.h"
#include "notice_helper.h"

RD_NAMESPACE_BEGIN

  static size_t s_pool_size = 0;
  static bool s_enable_cpu_affinity = true;

  INSTANCE_IMP(EventPollerPool)

  EventPoller::Ptr EventPollerPool::getFirstPoller() {
    return static_pointer_cast<EventPoller>(_threads.front());
  }

  EventPoller::Ptr EventPollerPool::getPoller(bool prefer_current_thread) {
    auto poller = EventPoller::getCurrentPoller();
    if (prefer_current_thread && _prefer_current_thread && poller) {
      return poller;
    }
    return static_pointer_cast<EventPoller>(GetExecutor());
  }

  void EventPollerPool::preferCurrentThread(bool flag) {
    _prefer_current_thread = flag;
  }

  const std::string EventPollerPool::kOnStarted = "kBroadcastEventPollerPoolStarted";

  EventPollerPool::EventPollerPool() {
    auto size = addPoller("event poller", s_pool_size, ThreadPool::PRIORITY_HIGHEST, true, s_enable_cpu_affinity);
    NOTICE_EMIT(EventPollerPoolOnStartedArgs, kOnStarted, *this, size);
    InfoL << "EventPoller created Size: " << size;
  }

  void EventPollerPool::setPoolSize(size_t size) {
    s_pool_size = size;
  }

  void EventPollerPool::enableCpuAffinity(bool enable) {
    s_enable_cpu_affinity = enable;
  }

RD_NAMESPACE_END