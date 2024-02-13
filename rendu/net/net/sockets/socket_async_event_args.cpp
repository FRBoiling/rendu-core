/*
* Created by boil on 2024/2/4.
*/

#include "socket_async_event_args.h"

NET_NAMESPACE_BEGIN


SocketError SocketAsyncEventArgs::GetSocketError() { return SocketError::ProcessLimit; }

void SocketAsyncEventArgs::SetBuffer(std::span<byte> buffer, int offset, int count) {
  if (buffer.empty()) {
    m_buffer = new Memory<byte>(nullptr, 0);
    m_offset = 0;
    m_count = 0;
    m_bufferIsExplicitArray = false;
  } else {
    if (!m_bufferList.empty()) {
      throw std::logic_error("Buffer and BufferList properties cannot both be non-null.");
    }
    if ((long) (uint) offset > (long) buffer.size()) {
      throw std::out_of_range("Offset is out of buffer bounds.");
    }
    if ((long) (uint) count > (long) (buffer.size() - offset)) {
      throw std::out_of_range("Count is out of buffer bounds.");
    }
    m_buffer = new Memory<byte>(buffer.data() + offset, count);
    m_offset = offset;
    m_count = count;
    m_bufferIsExplicitArray = true;
  }
}

void SocketAsyncEventArgs::SetBuffer(Memory<byte> *buffer) {
  if (buffer->IsEmpty()) {
    m_buffer = new Memory<byte>(nullptr, 0);
    m_offset = 0;
    m_count = 0;
    m_bufferIsExplicitArray = false;
  } else {
    if (!m_bufferList.empty()) {
      throw std::logic_error("Buffer and BufferList properties cannot both be non-null.");
    }
    m_buffer = buffer;
    m_offset = 0;
    m_count = buffer->Length();
    m_bufferIsExplicitArray = true;
  }
}

void SocketAsyncEventArgs::AcceptCompletionCallback(int *accepted_fd, Memory<byte> *socketAddress, SocketError &socketError) {
}

SocketError SocketAsyncEventArgs::DoOperationAccept(Socket *socket, SafeSocketHandle::Ptr handle, SafeSocketHandle::Ptr acceptHandle, CancellationToken &cancellationToken) {
  if (!m_buffer)
    throw std::logic_error("PlatformNotSupportedException be non-null.");
  m_accepted_fd = -1;
  int socketAddressLen;
  int acceptedFd;

  SocketError socketError = handle->m_async_context->AcceptAsync(
      new Memory<byte>(m_accept_buffer, sizeof(m_accept_buffer)),
      socketAddressLen,
      acceptedFd,
      [&](int *accepted_fd, Memory<byte> *socketAddress, SocketError &socketError) { AcceptCompletionCallback(accepted_fd, socketAddress, socketError); },
      cancellationToken);

  if (socketError != SocketError::IOPending) {
    Memory<byte> memory(m_accept_buffer, socketAddressLen);
    CompleteAcceptOperation(acceptedFd, memory, socketError);
    FinishOperationSync(socketError, 0, SocketFlags::None);
  }
  return socketError;
}

void SocketAsyncEventArgs::CompleteAcceptOperation(int accepted_fd, Memory<byte> &socketAddress, SocketError socketError) {
  m_accepted_fd = accepted_fd;
  if (socketError == SocketError::Success)
    m_accept_address_buffer_count = socketAddress.Length();
  else
    m_accept_address_buffer_count = 0;
}

void SocketAsyncEventArgs::FinishOperationSync(SocketError socketError, int bytesTransferred, SocketFlags flags) {
  if (socketError == SocketError::Success)
    FinishOperationSyncSuccess(bytesTransferred, flags);
  else
    FinishOperationSyncFailure(socketError, bytesTransferred, flags);
  if (GetLastOperation() > SocketAsyncOperation::Connect)
    return;
}

void SocketAsyncEventArgs::FinishOperationSyncSuccess(int bytesTransferred, SocketFlags flags) {
  //TODO：BOIL
}

void SocketAsyncEventArgs::FinishOperationSyncFailure(SocketError socketError, int bytesTransferred, SocketFlags flags) {
  //TODO：BOIL
}

void SocketAsyncEventArgs::StartOperationCommon(Socket *socket, SocketAsyncOperation operation) {
  int expected = 0;
  int desired = 1;
  bool status = m_operating.compare_exchange_strong(expected, desired);
  if (!status)
    ThrowForNonFreeStatus(expected);

  m_completed_operation = operation;
  m_current_socket = socket;
  if (!m_flow_execution_context && (operation != SocketAsyncOperation::Connect && operation != SocketAsyncOperation::Accept))
    return;
  m_context = ExecutionContext::Capture();
}

void SocketAsyncEventArgs::StartOperationAccept() {
  if (!m_buffer->IsEmpty()) {
    if (m_count < m_accept_address_buffer_count)
      throw std::invalid_argument("Count is too small");
  } else {
    if (m_accept_buffer != nullptr && sizeof(m_accept_buffer) >= m_accept_address_buffer_count)
      return;
    m_accept_buffer = new byte[m_accept_address_buffer_count];
  }
};

void SocketAsyncEventArgs::Complete() {
  //TODO：BOIL
}


void SocketAsyncEventArgs::ThrowForNonFreeStatus(int status)
{
  //TODO:BOIL
//  ObjectDisposedException.ThrowIf(status == 2, (object) this);
//  throw new InvalidOperationException(SR.net_socketopinprogress);
}

NET_NAMESPACE_END