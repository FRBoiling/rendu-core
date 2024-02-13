/*
* Created by boil on 2024/2/4.
*/

#ifndef RENDU_NET_NET_SOCKETS_SOCKET_ASYNC_EVENT_ARGS_H_
#define RENDU_NET_NET_SOCKETS_SOCKET_ASYNC_EVENT_ARGS_H_

#include "net_define.h"

#include "endpoint/ip_end_point.h"
#include "socket.h"
#include "socket_async_context.h"
#include "socket_async_operation.h"
#include "socket_error.h"
#include "socket_flags.h"

NET_NAMESPACE_BEGIN

class SocketAsyncEventArgs : public EventArgs {

public:
  SocketAsyncEventArgs()
      : m_buffer(new Memory<byte>()) {
  }

public:
  EventHandler<SocketAsyncEventArgs> Completed;

private:
  IPEndPoint *m_remote_end_point;
  Socket *m_accept_socket;
  int m_bytes_transferred;
  Memory<byte> *m_buffer;
  int m_offset;
  int m_count;
  bool m_bufferIsExplicitArray;
  std::vector<std::span<byte>> m_bufferList;

  int m_accepted_fd;

  byte *m_accept_buffer;
  int m_accept_address_buffer_count;
  SocketAsyncOperation m_completed_operation;
  Socket *m_current_socket;
  bool m_flow_execution_context;

  ExecutionContext* m_context;

  std::atomic<int> m_operating;

public:
  SocketError GetSocketError();

  void SetRemoteEndPoint(IPEndPoint *remote_end_point) { m_remote_end_point = remote_end_point; }
  IPEndPoint *GetRemoteEndPoint() { return m_remote_end_point; }

  void SetBuffer(std::span<byte> buffer, int offset, int count);
  void SetBuffer(Memory<byte> *buffer);

  int GetBytesTransferred() { return m_bytes_transferred; };
  void SetBytesTransferred(int transferred) { m_bytes_transferred = transferred; };

  Socket *GetAcceptSocket() { return m_accept_socket; };
  void SetAcceptSocket(Socket *socket) { m_accept_socket = socket; };

  void StartOperationCommon(Socket *socket, SocketAsyncOperation operation);
  void StartOperationAccept();
  SocketError DoOperationAccept(Socket *socket, SafeSocketHandle::Ptr handle, SafeSocketHandle::Ptr acceptHandle, CancellationToken &cancellationToken);
  void Complete();
  void CompleteAcceptOperation(int accepted_fd, Memory<byte> &socketAddress, SocketError socketError);
  void FinishOperationSync(SocketError socketError, int bytesTransferred, SocketFlags flags);
  void FinishOperationSyncSuccess(int bytesTransferred, SocketFlags flags);
  void FinishOperationSyncFailure(SocketError socketError, int bytesTransferred, SocketFlags flags);
  void FinishOperationAsync(SocketError socketError, int bytesTransferred, SocketFlags flags);

  SocketAsyncOperation GetLastOperation() { return m_completed_operation; }
  void AcceptCompletionCallback(int *accepted_fd, Memory<byte> *socketAddress, SocketError &socketError);

private:
  void ThrowForNonFreeStatus(int status);

};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_SOCKET_ASYNC_EVENT_ARGS_H_
