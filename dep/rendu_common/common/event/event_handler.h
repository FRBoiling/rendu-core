/*
* Created by boil on 2023/10/17.
*/

#ifndef RENDU_EVENT_HANDLER_H
#define RENDU_EVENT_HANDLER_H

#include "common/define.h"


RD_NAMESPACE_BEGIN

// 定义一个事件类
  template<typename T>
  class EventHandler {
  private:
    std::vector<std::function<void(void *, T *)>> handlers; // 存储函数对象的容器
  public:
    // 添加事件处理器
    EventHandler &operator+=(std::function<void(void *, T *)> handler) {
      handlers.push_back(handler);
      return *this;
    }

    // 移除事件处理器
    EventHandler &operator-=(std::function<void(void *, T *)> handler) {
      handlers.erase(std::remove(handlers.begin(), handlers.end(), handler), handlers.end());
      return *this;
    }

    // 触发事件
    void operator()(void *sender, T *args) {
      for (auto &handler: handlers) {
        handler(sender, args);
      }
    }
  };

RD_NAMESPACE_END

#endif //RENDU_EVENT_HANDLER_H
