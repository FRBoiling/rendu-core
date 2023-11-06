/*
* Created by boil on 2023/10/25.
*/

#include "socket_num.h"

RD_NAMESPACE_BEGIN

  SocketNum::SocketNum(int fd, SocketNum::SockType type) {
    _fd = fd;
    _type = type;
  }

  SocketNum::SockType SocketNum::GetType() {
    return _type;
  }

  int32 SocketNum::RawFd() const {
    return _fd;
  }

  void SocketNum::SetConnected() {

  }

RD_NAMESPACE_END