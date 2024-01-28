/*
* Created by boil on 2023/10/17.
*/

#ifndef RENDU_EVENT_HANDLER_H
#define RENDU_EVENT_H

#include <functional>
#include "socket_async_event_args.h"

// 定义一个事件类
template<typename T>
class Event {
private:
  std::vector<std::function<void(void *, T *)>> handlers; // 存储函数对象的容器
public:
  // 添加事件处理器
  Event &operator+=(std::function<void(void *, T *)> handler) {
    handlers.push_back(handler);
    return *this;
  }

  // 移除事件处理器
  Event &operator-=(std::function<void(void *, T *)> handler) {
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

//// 使用示例
//class Socket {
//public:
//  Event<SocketAsyncEventArgs> Completed; // 定义一个名为 Completed 的事件
//
//  void AsyncOperation() {
////    // 模拟异步操作
////    std::this_thread::sleep_for(std::chrono::seconds(1));
//    // 创建一个 SocketAsyncEventArgs 对象
//    SocketAsyncEventArgs args;
////    args.BytesTransferred = 1024;
////    args.SocketError = 0;
//    // 触发 Completed 事件
//
//    Completed(this, &args);
//  }
//};


#endif //RENDU_EVENT_HANDLER_H
