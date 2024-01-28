/*
* Created by boil on 2023/3/26.
*/

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include "event.h"
#include <iostream>

void OnComplete(void* sender,SocketAsyncEventArgs* args) {
//        m_service->Queue.Enqueue(new TArgs() { ChannelId = this.Id, SocketAsyncEventArgs = args });
}

int main() {
  // 创建一个 Socket 对象
  Socket socket;
  socket.Completed+= OnComplete;
  // 添加一个 lambda 表达式作为事件处理器
  socket.Completed += [](void *sender, SocketAsyncEventArgs *args) {
    std::cout << "Async operation completed.\n";
//    std::cout << "Bytes transferred: " << args->BytesTransferred << "\n";
//    std::cout << "Socket error: " << args->SocketError << "\n";
  };
  // 启动异步操作
  socket.AsyncOperation();
  return 0;
}

