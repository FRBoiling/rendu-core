/*
* Created by boil on 2023/11/2.
*/

#include "timer.h"
#include "utils/atomic.h"
#include "event_loop_pool.h"

RD_NAMESPACE_BEGIN

  Timer::Timer(float second, const std::function<bool()> &cb, const EventLoop::Ptr &poller) {
    _poller = poller;
    if (!_poller) {
      _poller = EventLoopPool::Instance().getPoller();
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
        LOG_ERROR << "Exception occurred when do timer task: " << ex.what();
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

//  AtomicInt64 Timer::s_numCreated_;
//
//  void Timer::restart(Timestamp now) {
//    if (repeat_) {
//      expiration_ = addTime(now, interval_);
//    } else {
//      expiration_ = Timestamp::invalid();
//    }
//  }

RD_NAMESPACE_END