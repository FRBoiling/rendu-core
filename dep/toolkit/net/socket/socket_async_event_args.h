/*
* Created by boil on 2023/10/24.
*/

#ifndef RENDU_SOCKET_ASYNC_EVENT_ARGS_H
#define RENDU_SOCKET_ASYNC_EVENT_ARGS_H

#include "define.h"
#include "socket.h"
#include "net/event_handler.h"

RD_NAMESPACE_BEGIN

  class SocketAsyncEventArgs {
  private:
    Socket::Ptr _acceptSocket;
    Socket::Ptr _connectSocket;
    SocketAsyncOperation _completedOperation;
    IPEndPoint _remoteEndPoint;

public:
    EventHandler<SocketAsyncEventArgs> Completed;//发生错误回调

  };

RD_NAMESPACE_END
#endif //RENDU_SOCKET_ASYNC_EVENT_ARGS_H
