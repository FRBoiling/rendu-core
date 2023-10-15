/*
* Created by boil on 2023/10/27.
*/

#include "buffer_socket.h"
#include "socket/socket_util.h"

RD_NAMESPACE_BEGIN

  BufferSocket::BufferSocket(ABuffer::Ptr buffer, struct sockaddr *addr, int addr_len) {
    if (addr) {
      _addr_len = addr_len ? addr_len : SockUtil::get_sock_len(addr);
      memcpy(&_addr, addr, _addr_len);
    }
    assert(buffer);
    _buffer = std::move(buffer);
  }

  char *BufferSocket::Data() const {
    return _buffer->Data();
  }

  size_t BufferSocket::Size() const {
    return _buffer->Size();
  }

  const struct sockaddr *BufferSocket::GetSockAddr() const {
    return (struct sockaddr *)&_addr;
  }

  socklen_t BufferSocket::GetSockLen() const {
    return _addr_len;
  }


RD_NAMESPACE_END