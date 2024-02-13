/*
* Created by boil on 2024/2/18.
*/

#include "safe_socket_handle.h"
#include "socket_async_context.h"

NET_NAMESPACE_BEGIN
SafeSocketHandle::SafeSocketHandle()
    : SafeSocketHandle(SocketType::Raw) {}

SafeSocketHandle::SafeSocketHandle(SocketType type): m_type(type){}

SafeSocketHandle::SafeSocketHandle(int pre_existing_fd, bool ) : interop::SafeHandle{pre_existing_fd} {}

SafeSocketHandle::~SafeSocketHandle() { Close(); };

NET_NAMESPACE_END