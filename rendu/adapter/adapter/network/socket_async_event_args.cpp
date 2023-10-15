/*
* Created by boil on 2023/10/16.
*/

#include "socket_async_event_args.h"
#include "socket_error.h"

RD_NAMESPACE_BEGIN

    void SocketAsyncEventArgs::SetRemoteEndPoint(IPEndPoint *ipEndPoint) {
      m_ipEndPoint = ipEndPoint;
    }

    SocketError SocketAsyncEventArgs::GetSocketError() {

      return SocketError::Success;
    }

    void SocketAsyncEventArgs::SetBuffer(byte *pByte, int64 i, int i1) {

    }

    int SocketAsyncEventArgs::GetBytesTransferred() {
      return 0;
    }

    SocketAsyncOperation SocketAsyncEventArgs::GetLastOperation() {
      return SocketAsyncOperation::None;
    }

    Socket *SocketAsyncEventArgs::GetAcceptSocket() {
      return nullptr;
    }

RD_NAMESPACE_END