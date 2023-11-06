/*
* Created by boil on 2023/10/26.
*/

#include "socket_wrapper.h"

RD_NAMESPACE_BEGIN

  SocketWrapper::SocketWrapper(const Socket::Ptr &sock) {
    setSock(sock);
    setOnCreateSocket(nullptr);
  }

  void SocketWrapper::setPoller(const EventLoop::Ptr &poller) {
    _poller = poller;
  }

  void SocketWrapper::setSock(const Socket::Ptr &sock) {
    _sock = sock;
    if (_sock) {
      _poller = _sock->getPoller();
    }
  }

  const EventLoop::Ptr &SocketWrapper::getPoller() const {
    assert(_poller);
    return _poller;
  }

  const Socket::Ptr &SocketWrapper::getSock() const {
    return _sock;
  }

  int SocketWrapper::flushAll() {
    if (!_sock) {
      return -1;
    }
    return _sock->flushAll();
  }

  ssize_t SocketWrapper::Send(ABuffer::Ptr buf) {
    if (!_sock) {
      return -1;
    }
    return _sock->send(std::move(buf), nullptr, 0, _try_flush);
  }

  void SocketWrapper::Shutdown(const SockException &ex) {
    if (_sock) {
      _sock->emitErr(ex);
    }
  }

  void SocketWrapper::safeShutdown(const SockException &ex) {
    std::weak_ptr<SocketWrapper> weak_self = shared_from_this();
    AsyncFirst([weak_self, ex]() {
      if (auto strong_self = weak_self.lock()) {
        strong_self->Shutdown(ex);
      }
    });
  }

  string SocketWrapper::GetLocalIp() {
    return _sock ? _sock->GetLocalIp() : "";
  }

  uint16_t SocketWrapper::GetLocalPort() {
    return _sock ? _sock->GetLocalPort() : 0;
  }

  string SocketWrapper::GetRemoteIp() {
    return _sock ? _sock->GetRemoteIp() : "";
  }

  uint16_t SocketWrapper::GetRemotePort() {
    return _sock ? _sock->GetRemotePort() : 0;
  }

  bool SocketWrapper::isSocketBusy() const {
    if (!_sock) {
      return true;
    }
    return _sock->isSocketBusy();
  }

  Task::Ptr SocketWrapper::Async(TaskIn task, bool may_sync) {
    return _poller->Async(std::move(task), may_sync);
  }

  Task::Ptr SocketWrapper::AsyncFirst(TaskIn task, bool may_sync) {
    return _poller->AsyncFirst(std::move(task), may_sync);
  }

  void SocketWrapper::setSendFlushFlag(bool try_flush) {
    _try_flush = try_flush;
  }

  void SocketWrapper::setSendFlags(int flags) {
    if (!_sock) {
      return;
    }
    _sock->setSendFlags(flags);
  }

  void SocketWrapper::setOnCreateSocket(Socket::onCreateSocket cb) {
    if (cb) {
      _on_create_socket = std::move(cb);
    } else {
      _on_create_socket = [](const EventLoop::Ptr &poller) { return Socket::CreateSocket(poller, false); };
    }
  }

  Socket::Ptr SocketWrapper::createSocket() {
    return _on_create_socket(_poller);
  }

  IPEndPoint *SocketWrapper::GetEndPoint() {
    return nullptr;
  }

  void SocketWrapper::Bind(IPEndPoint *endPoint) {

  }

  void SocketWrapper::Close() {

  }

  void SocketWrapper::Listen(int backlog) {

  }

  void SocketWrapper::Connect(const char *remote_host, uint16_t remote_port) {

  }

  ISocket *SocketWrapper::Accept() {
    return nullptr;
  }

  string SocketWrapper::GetIdentifier() const {
    return ISocket::GetIdentifier();
  }

RD_NAMESPACE_END