/*
* Created by boil on 2023/10/29.
*/

#ifndef RENDU_NOTICE_CENTER_H
#define RENDU_NOTICE_CENTER_H

#include "define.h"
#include "event_dispatcher.h"

RD_NAMESPACE_BEGIN

class NoticeCenter : public std::enable_shared_from_this<NoticeCenter> {
public:
  using Ptr = std::shared_ptr<NoticeCenter>;

  static NoticeCenter &Instance();

  template <typename... ArgsType>
  int emitEvent(const std::string &event, ArgsType &&...args) {
    return emitEvent_l(false, event, std::forward<ArgsType>(args)...);
  }

  template <typename... ArgsType>
  int emitEventSafe(const std::string &event, ArgsType &&...args) {
    return emitEvent_l(true, event, std::forward<ArgsType>(args)...);
  }

  template <typename FUNC>
  void addListener(void *tag, const std::string &event, FUNC &&func) {
    getDispatcher(event, true)->addListener(tag, std::forward<FUNC>(func));
  }

  void delListener(void *tag, const std::string &event) {
    auto dispatcher = getDispatcher(event);
    if (!dispatcher) {
      // 不存在该事件
      return;
    }
    bool empty;
    dispatcher->delListener(tag, empty);
    if (empty) {
      delDispatcher(event, dispatcher);
    }
  }

  // 这个方法性能比较差
  void delListener(void *tag) {
    std::lock_guard<std::recursive_mutex> lck(_mtxListener);
    bool empty;
    for (auto it = _mapListener.begin(); it != _mapListener.end();) {
      it->second->delListener(tag, empty);
      if (empty) {
        it = _mapListener.erase(it);
        continue;
      }
      ++it;
    }
  }

  void clearAll() {
    std::lock_guard<std::recursive_mutex> lck(_mtxListener);
    _mapListener.clear();
  }

private:
  template <typename... ArgsType>
  int emitEvent_l(bool safe, const std::string &event, ArgsType &&...args) {
    auto dispatcher = getDispatcher(event);
    if (!dispatcher) {
      // 该事件无人监听
      return 0;
    }
    return dispatcher->emitEvent(safe, std::forward<ArgsType>(args)...);
  }

  EventDispatcher::Ptr getDispatcher(const std::string &event, bool create = false) {
    std::lock_guard<std::recursive_mutex> lck(_mtxListener);
    auto it = _mapListener.find(event);
    if (it != _mapListener.end()) {
      return it->second;
    }
    if (create) {
      // 如果为空则创建一个
      EventDispatcher::Ptr dispatcher(new EventDispatcher());
      _mapListener.emplace(event, dispatcher);
      return dispatcher;
    }
    return nullptr;
  }

  void delDispatcher(const std::string &event, const EventDispatcher::Ptr &dispatcher) {
    std::lock_guard<std::recursive_mutex> lck(_mtxListener);
    auto it = _mapListener.find(event);
    if (it != _mapListener.end() && dispatcher == it->second) {
      // 两者相同则删除
      _mapListener.erase(it);
    }
  }

private:
  std::recursive_mutex _mtxListener;
  std::unordered_map<std::string, EventDispatcher::Ptr> _mapListener;
};

RD_NAMESPACE_END

#endif //RENDU_NOTICE_CENTER_H
