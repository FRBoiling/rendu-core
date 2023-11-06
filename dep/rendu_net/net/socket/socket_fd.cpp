/*
* Created by boil on 2023/10/25.
*/

#include "socket_fd.h"

RD_NAMESPACE_BEGIN

  SocketFD::SocketFD(int num, SocketNum::SockType type) {
    _num = std::make_shared<SocketNum>(num, type);
  }

  SocketFD::SocketFD(const SocketFD &that) {
    _num = that._num;
  }

  SocketFD::~SocketFD() { DelEvent(); }

  void SocketFD::SetConnected() {
    _num->SetConnected();
  }

  int SocketFD::RawFd() const {
    return _num->RawFd();
  }

  SocketNum::SockType SocketFD::GetType() const {
    return _num->GetType();
  }



  void SocketFD::DelEvent() {
  }


RD_NAMESPACE_END