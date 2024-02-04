/*
* Created by boil on 2024/2/4.
*/

#include "socket_async_event_args.h"

NET_NAMESPACE_BEGIN

void SocketAsyncEventArgs::SetRemoteEndPoint(IPEndPoint *remote_end_point) { m_remote_end_point = remote_end_point; }

SocketError SocketAsyncEventArgs::GetSocketError() { return SocketError::ProcessLimit; }


NET_NAMESPACE_END