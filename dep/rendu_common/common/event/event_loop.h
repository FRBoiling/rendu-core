/*
* Created by boil on 2023/10/24.
*/

#ifndef RENDU_EVENT_LOOP_H
#define RENDU_EVENT_LOOP_H

#include "common/define.h"

#include "a_event.h"
#include "a_event_key.h"
#include <unordered_map>

RD_NAMESPACE_BEGIN

  class EventLoop {
  public:
    using Ptr = std::shared_ptr<EventLoop>;

    ~EventLoop();

  public:
    virtual void Loop() = 0;

    // 注册事件
    virtual int RegisterEvent(AEvent &event) = 0;
    // 事件
    virtual int AddFiredEvent(AEvent &event) = 0;

    virtual int DelFiredEvent(AEvent &event) = 0;

  private:
    // 标记loop线程是否退出
    bool _exit_flag;

    // registered events
    std::unordered_map<AEventKey, AEvent> _registered_events;

    // fired events
    std::unordered_map<AEventKey, AEvent> _fired_events;
  };

RD_NAMESPACE_END

#endif //RENDU_EVENT_LOOP_H
