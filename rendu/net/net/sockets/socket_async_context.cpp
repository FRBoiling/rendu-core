/*
* Created by boil on 2024/3/18.
*/

#include "socket_async_context.h"

#include <utility>
#include "interop/sys.h"
#include "socket_exception.h"
#include "socket_pal.h"

NET_NAMESPACE_BEGIN


bool SocketAsyncContext::AcceptOperation::DoTryComplete(SocketAsyncContext *context){
  //TODO：BOIL
  return false;
};


SocketError SocketAsyncContext::AcceptAsync(Memory<byte>* socketAddress, int &socketAddressLen, int &acceptedFd, AcceptOperation::CallBack callback, CancellationToken cancellationToken) {
  SetHandleNonBlocking();
  int observedSequenceNumber;
  SocketError errorCode1;
  if (m_receive_queue.IsReady(this, observedSequenceNumber) && SocketPal::TryCompleteAccept(m_socket, *socketAddress, socketAddressLen, acceptedFd, errorCode1))
    return errorCode1;
  AcceptOperation operation(this);
  operation.m_callback = std::move(callback);
  operation.m_socket_address = socketAddress;
  if (!m_receive_queue.StartAsyncOperation(this, &operation, observedSequenceNumber, cancellationToken)) {
    socketAddressLen = operation.m_socket_address->Length();
    acceptedFd = operation.m_accepted_fd;
    SocketError errorCode2 = operation.m_error_code;
    ReturnOperation(operation);
    return errorCode2;
  }
  acceptedFd = -1;
  socketAddressLen = 0;
  return SocketError::IOPending;
}

void SocketAsyncContext::ReturnOperation(AcceptOperation& operation)
{
  operation.Reset();
  operation.m_callback = nullptr;
  operation.m_socket_address = nullptr;
  m_cached_accept_operation.store(&operation, std::memory_order_release);
}

void SocketAsyncContext::SetHandleNonBlocking() {
  if (!m_is_handle_non_blocking) {
    if (interop::Sys::Fcntl::SetIsNonBlocking(m_socket, 1) != 0) {
      throw new SocketException(SocketPal::GetSocketErrorForErrorCode(interop::Sys::GetLastError()));
    }
    m_is_handle_non_blocking = true;
  }
}


NET_NAMESPACE_END