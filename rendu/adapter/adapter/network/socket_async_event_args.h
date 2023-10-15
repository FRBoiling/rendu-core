/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_SOCKET_ASYNC_EVENT_ARGS_H
#define RENDU_SOCKET_ASYNC_EVENT_ARGS_H

#include "define.h"
#include "event/event_handler.h"
#include "ip_end_point.h"
#include "socket_error.h"
#include "socket_type.h"
#include "socket.h"

RD_NAMESPACE_BEGIN

    class SocketAsyncEventArgs {
    public:
      EventHandler<SocketAsyncEventArgs> Completed;
    public:
      void SetRemoteEndPoint(IPEndPoint *ipAddress);

      SocketError GetSocketError();

      void SetBuffer(byte *pByte, int64 i, int i1);

      int GetBytesTransferred();

      SocketAsyncOperation GetLastOperation();

      Socket *m_AcceptSocket{};

      Socket *GetAcceptSocket();

    private:
      IPEndPoint *m_ipEndPoint;
      SocketError m_socketError;
    };

RD_NAMESPACE_END

#endif //RENDU_SOCKET_ASYNC_EVENT_ARGS_H
