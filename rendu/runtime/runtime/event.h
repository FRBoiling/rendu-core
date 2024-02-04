/*
* Created by boil on 2024/2/5.
*/

#ifndef RENDU_RUNTIME_RUNTIME_EVENT_H_
#define RENDU_RUNTIME_RUNTIME_EVENT_H_

#include "runtime_define.h"
#include "event_args.h"

RUNTIME_NAMESPACE_BEGIN

class Object{};

template <typename TEventArgs>
class EventHandler {
public:
  // 定义成员函数指针类型
  using EventFunc = std::function<void(void *, TEventArgs *)>;
  EventHandler& operator+=(EventFunc func) {
      listeners.push_back(func);
      return *this;
  }
private:
  std::vector<EventFunc> listeners;
};

template <typename TEventArgs>
class Event {
public:
//  void Subscribe(const EventHandler<TEventArgs>& handler) {
//    listeners.push_back(handler);
//  }
//
//  void Fire(const EventArgs& args) {
//    for (const auto& listener : listeners) {
//      listener(args);
//    }
//  }

private:
//  std::vector<EventHandler> listeners;
};

RUNTIME_NAMESPACE_END

#endif//RENDU_RUNTIME_RUNTIME_EVENT_H_
