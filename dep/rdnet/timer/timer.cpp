/*
* Created by boil on 2023/10/26.
*/

#include "timer.h"
#include "poller/event_poller_pool.h"
#include "logger/logger.h"

RD_NAMESPACE_BEGIN

  Timer::Timer(float second, const std::function<bool()> &cb, const EventPoller::Ptr &poller) {
    _poller = poller;
    if (!_poller) {
      _poller = EventPollerPool::Instance().getPoller();
    }
    _tag = _poller->doDelayTask((uint64_t) (second * 1000), [cb, second]() {
      try {
        if (cb()) {
          //重复的任务
          return (uint64_t) (1000 * second);
        }
        //该任务不再重复
        return (uint64_t) 0;
      } catch (std::exception &ex) {
        ErrorL << "Exception occurred when do timer task: " << ex.what();
        return (uint64_t) (1000 * second);
      }
    });
  }

  Timer::~Timer() {
    auto tag = _tag.lock();
    if (tag) {
      tag->Cancel();
    }
  }

RD_NAMESPACE_END