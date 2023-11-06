/*
* Created by boil on 2023/10/25.
*/

#include "socket_fd.h"

RD_NAMESPACE_BEGIN

  SocketFD::SocketFD(int num, SocketNum::SockType type, const EventLoop::Ptr &poller) {
    _num = std::make_shared<SocketNum>(num, type);
    _poller = poller;
  }

  SocketFD::SocketFD(const SocketFD &that, const EventLoop::Ptr &poller) {
    _num = that._num;
    _poller = poller;
    if (_poller == that._poller) {
      throw std::invalid_argument("Copy a SockFD with same poller");
    }
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

  const EventLoop::Ptr &SocketFD::GetPoller() const {
    return _poller;
  }

  void SocketFD::DelEvent() {
    if (_poller) {
      auto num = _num;
      // 移除io事件成功后再close fd
      _poller->RemoveEvent(num->RawFd(), [num](bool) {});
      _poller = nullptr;
    }
  }


RD_NAMESPACE_END