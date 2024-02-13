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
  IPEndPoint *GetRemoteEndPoint();

  void SetBuffer(std::vector<byte> &buffer, int offset, int count);
  void SetBuffer(Memory<byte> buffer);
  void SetBuffer(std::span<byte> buffer);

  int GetBytesTransferred(){return m_bytes_transferred;};
  void SetBytesTransferred(int transferred){  m_bytes_transferred = transferred;};
  SocketAsyncOperation GetLastOperation() { return m_last_operation; };


  Socket *GetAcceptSocket() { return m_accept_socket; };
  void SetAcceptSocket(Socket *socket) { m_accept_socket = socket; };

private:
  IPEndPoint *m_remote_end_point;
  Socket *m_accept_socket;
  SocketAsyncOperation m_last_operation;
  int m_bytes_transferred;
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_SOCKET_ASYNC_EVENT_ARGS_H_
