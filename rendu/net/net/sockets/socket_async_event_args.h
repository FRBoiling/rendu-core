/*
* Created by boil on 2024/2/4.
*/

#ifndef RENDU_NET_NET_SOCKETS_SOCKET_ASYNC_EVENT_ARGS_H_
#define RENDU_NET_NET_SOCKETS_SOCKET_ASYNC_EVENT_ARGS_H_

#include "net_define.h"

#include "endpoint/ip_end_point.h"
#include "socket.h"
#include "socket_async_operation.h"
#include "socket_error.h"

NET_NAMESPACE_BEGIN

class SocketAsyncEventArgs : public EventArgs {

public:
  EventHandler<SocketAsyncEventArgs> Completed;
  SocketError GetSocketError();

  void SetRemoteEndPoint(IPEndPoint *remote_end_point);

  void SetBuffer(BYTE *buffer, int offset, int count);

  INT32 GetBytesTransferred();
  SocketAsyncOperation GetLastOperation();


  Socket *GetAcceptSocket();
  Socket *SetAcceptSocket(Socket *socket);

private:
  IPEndPoint *m_remote_end_point;
  Socket *m_accept_socket;
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_SOCKET_ASYNC_EVENT_ARGS_H_
