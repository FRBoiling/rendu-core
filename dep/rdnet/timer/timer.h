/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_TIMER_H
#define RENDU_TIMER_H

#include "poller/event_poller.h"

RD_NAMESPACE_BEGIN

  class Timer {
  public:
    using Ptr = std::shared_ptr<Timer>;

    /**
     * 构造定时器
     * @param second 定时器重复秒数
     * @param cb 定时器任务，返回true表示重复下次任务，否则不重复，如果任务中抛异常，则默认重复下次任务
     * @param poller EventPoller对象，可以为nullptr
     */
    Timer(float second, const std::function<bool()> &cb, const EventPoller::Ptr &poller);

    ~Timer();

  private:
    std::weak_ptr<EventPoller::DelayTask> _tag;
    //定时器保持EventPoller的强引用
    EventPoller::Ptr _poller;
  };

RD_NAMESPACE_END

#endif //RENDU_TIMER_H
