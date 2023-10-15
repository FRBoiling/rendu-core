/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_EVENT_DISPATCHER_H
#define RENDU_EVENT_DISPATCHER_H

#include "define.h"
#include <mutex>
#include <memory>
#include <string>
#include <exception>
#include <functional>
#include <unordered_map>
#include <stdexcept>
#include "utils/any.h"
#include "utils/function_traits.h"

RD_NAMESPACE_BEGIN

  class EventDispatcher {

  public:
    friend class NoticeCenter;

    using Ptr = std::shared_ptr<EventDispatcher>;

    ~EventDispatcher() = default;

  private:
    using MapType = std::unordered_multimap<void *, Any>;

    EventDispatcher() = default;

    class InterruptException : public std::runtime_error {
    public:
      InterruptException() : std::runtime_error("InterruptException") {}

      ~InterruptException() {}
    };

    template<typename... ArgsType>
    int emitEvent(bool safe, ArgsType &&...args) {
      using stl_func = std::function<void(decltype(std::forward<ArgsType>(args))...)>;
      decltype(_mapListener) copy;
      {
        // 先拷贝(开销比较小)，目的是防止在触发回调时还是上锁状态从而导致交叉互锁
        std::lock_guard<std::recursive_mutex> lck(_mtxListener);
        copy = _mapListener;
      }

      int ret = 0;
      for (auto &pr: copy) {
        try {
          pr.second.get<stl_func>(safe)(std::forward<ArgsType>(args)...);
          ++ret;
        } catch (InterruptException &) {
          ++ret;
          break;
        }
      }
      return ret;
    }

    template<typename FUNC>
    void addListener(void *tag, FUNC &&func) {
      using stl_func = typename FunctionTraits<typename std::remove_reference<FUNC>::type>::stl_function_type;
      Any listener;
      listener.set<stl_func>(std::forward<FUNC>(func));
      std::lock_guard<std::recursive_mutex> lck(_mtxListener);
      _mapListener.emplace(tag, listener);
    }

    void delListener(void *tag, bool &empty) {
      std::lock_guard<std::recursive_mutex> lck(_mtxListener);
      _mapListener.erase(tag);
      empty = _mapListener.empty();
    }

  private:
    std::recursive_mutex _mtxListener;
    MapType _mapListener;
  };

RD_NAMESPACE_END

#endif //RENDU_EVENT_DISPATCHER_H
