/*
* Created by boil on 2023/10/22.
*/

#include "socket.h"
#include "utils/socket_helper.h"
#include "thread/mutex_wrapper.h"
#include "thread/work_thread_pool.h"
#include "exception/socket_exception.h"
#include "buffer_socket.h"
#include "buffer/buffer_string.h"

RD_NAMESPACE_BEGIN
  StatisticImp(Socket)

#define LOCK_GUARD(mtx) std::lock_guard<decltype(mtx)> lck(mtx)

  Socket::Socket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType)
    : ISocket(addressFamily, socketType, protocolType), m_ipEndpoint(nullptr), _sock_flags(SOCKET_DEFAULE_FLAGS),
      _mtx_sock_fd(false), _mtx_event(false), _mtx_send_buf_waiting(false),
      _mtx_send_buf_sending(false) {
  }

  Socket::Socket(const EventLoop::Ptr &poller, bool enable_mutex)
    : _mtx_sock_fd(enable_mutex), _mtx_event(enable_mutex), _mtx_send_buf_waiting(enable_mutex),
      _mtx_send_buf_sending(enable_mutex) {

    _poller = poller;
    if (!_poller) {
      _poller = EventLoopPool::Instance().getPoller();
    }
    setOnRead(nullptr);
    setOnErr(nullptr);
    setOnAccept(nullptr);
    setOnFlush(nullptr);
    setOnBeforeAccept(nullptr);
    setOnSendResult(nullptr);
  }

  Socket::~Socket() {
    closeSock();
  }


  IPEndPoint *Socket::GetEndPoint() {
    return m_ipEndpoint;
  }

  void Socket::Bind(IPEndPoint *endPoint) {
    m_ipEndpoint = static_cast<IPEndPoint *>(endPoint);
    if (m_ipEndpoint->IsIPv4()) {
      m_sfd = SocketHelper::Bind(m_ipEndpoint->GetPort(), m_ipEndpoint->GetAddress().ToString().c_str(), AF_INET);
    } else {
      m_sfd = SocketHelper::Bind(m_ipEndpoint->GetPort(), m_ipEndpoint->GetAddress().ToString().c_str(), AF_INET6);
    }
  }

  void Socket::Listen(int backlog) {
    SocketHelper::Listen(m_sfd, backlog);
  }

  ISocket *Socket::Accept() {
    int fd = SocketHelper::Accept(m_sfd);
    auto *socket = new Socket(m_af, m_st, m_pt);
    socket->m_sfd = fd;
    return socket;
  }

  void Socket::Close() {
    SocketHelper::Close(m_sfd);
  }

  void Socket::Connect(const char *remote_host, uint16_t remote_port) {
    m_sfd = SocketHelper::Connect(remote_host, remote_port);

  }

  Socket::Ptr Socket::CreateSocket(const EventLoop::Ptr &poller, bool enable_mutex) {
    return std::make_shared<Socket>(poller, enable_mutex);
  }

  void Socket::setOnRead(onReadCB cb) {
    LOCK_GUARD(_mtx_event);
    if (cb) {
      _on_read = std::move(cb);
    } else {
      _on_read = [](const ABuffer::Ptr &buf, struct sockaddr *, int) {
        LOG_WARN << "Socket not Set read callback, Data ignored: " << buf->Size();
      };
    }
  }

  void Socket::setOnErr(onErrCB cb) {
    LOCK_GUARD(_mtx_event);
    if (cb) {
      _on_err = std::move(cb);
    } else {
      _on_err = [](const SockException &err) {
        LOG_WARN << "Socket not set err callback, err: " << err;
      };
    }
  }

  void Socket::setOnAccept(onAcceptCB cb) {
    LOCK_GUARD(_mtx_event);
    if (cb) {
      _on_accept = std::move(cb);
    } else {
      _on_accept = [](Socket::Ptr &sock, std::shared_ptr<void> &complete) {
        LOG_WARN << "Socket not set accept callback, peer fd: " << sock->rawFD();
      };
    }
  }

  void Socket::setOnFlush(onFlush cb) {
    LOCK_GUARD(_mtx_event);
    if (cb) {
      _on_flush = std::move(cb);
    } else {
      _on_flush = []() {
        LOG_WARN << "Socket not set flush callback";
        return true;
      };
    }
  }

  void Socket::setOnBeforeAccept(onCreateSocket cb) {
    LOCK_GUARD(_mtx_event);
    if (cb) {
      _on_before_accept = std::move(cb);
    } else {
      _on_before_accept = [](const EventLoop::Ptr &poller) { return nullptr; };
    }
  }

  void Socket::setOnSendResult(onSendResult cb) {
    LOCK_GUARD(_mtx_event);
    _send_result = std::move(cb);
  }

#define CLOSE_SOCK(fd) if (fd != -1) { close(fd);}

  void
  Socket::connect(const string &url, uint16_t port, const onErrCB &con_cb_in, float timeout_sec, const string &local_ip,
                  uint16_t local_port) {
    std::weak_ptr<Socket> weak_self = shared_from_this();
    // 因为涉及到异步回调，所以在poller线程中执行确保线程安全
    _poller->Async([=] {
      if (auto strong_self = weak_self.lock()) {
        strong_self->connect_l(url, port, con_cb_in, timeout_sec, local_ip, local_port);
      }
    });
  }

  void Socket::connect_l(const string &url, uint16_t port, const onErrCB &con_cb_in, float timeout_sec,
                         const string &local_ip, uint16_t local_port) {
    // 重置当前socket
    closeSock();

    std::weak_ptr<Socket> weak_self = shared_from_this();
    auto con_cb = [con_cb_in, weak_self](const SockException &err) {
      auto strong_self = weak_self.lock();
      if (!strong_self) {
        return;
      }
      strong_self->_async_con_cb = nullptr;
      strong_self->_con_timer = nullptr;
      if (err) {
        LOCK_GUARD(strong_self->_mtx_sock_fd);
        strong_self->_sock_fd = nullptr;
      }
      con_cb_in(err);
    };

    auto async_con_cb = std::make_shared<std::function<void(int)>>
      ([weak_self, con_cb](int sock) {
        auto strong_self = weak_self.lock();
        if (sock == -1 || !strong_self) {
          if (!strong_self) {
            CLOSE_SOCK(sock);
          } else {
            con_cb(SockException(Err_dns, GetErrorMsg(true)));
          }
          return;
        }

        // 监听该socket是否可写，可写表明已经连接服务器成功
        int result = strong_self->_poller->AddEvent(sock, EventLoop::Event_Write | EventLoop::Event_Error,
                                                    [weak_self, sock, con_cb](int event) {
                                                      if (auto strong_self = weak_self.lock()) {
                                                        // socket可写事件，说明已经连接服务器成功
                                                        strong_self->setSock(
                                                          strong_self->makeSock(sock, SocketNum::Sock_TCP));
                                                        strong_self->onConnected(sock, con_cb);
                                                      } else {
                                                        CLOSE_SOCK(sock);
                                                      }
                                                    });

        if (result == -1) {
          CLOSE_SOCK(sock);
          con_cb(SockException(Err_other, string ("add event to poller failed when start connect") + GetErrorMsg()));
        }
      });

    // 连接超时定时器
    _con_timer = std::make_shared<Timer>(timeout_sec, [weak_self, con_cb]() {
      con_cb(SockException(Err_timeout, strerror(RD_ETIMEDOUT)));
      return false;
    }, _poller);

    if (SocketHelper::IsIP(url.data())) {
      (*async_con_cb)(SocketHelper::Connect(url.data(), port, true, local_ip.data(), local_port));
    } else {
      auto poller = _poller;
      std::weak_ptr<std::function<void(int)>> weak_task = async_con_cb;
      WorkThreadPool::Instance().GetExecutor()->Async([url, port, local_ip, local_port, weak_task, poller]() {
        // 阻塞式dns解析放在后台线程执行
        int sock = SocketHelper::Connect(url.data(), port, true, local_ip.data(), local_port);
        poller->Async([sock, weak_task]() {
          if (auto strong_task = weak_task.lock()) {
            (*strong_task)(sock);
          } else {
            CLOSE_SOCK(sock);
          }
        });
      });
      _async_con_cb = async_con_cb;
    }
  }

  void Socket::onConnected(int sock, const onErrCB &cb) {
    auto err = GetSockErr(sock, false);
    if (err) {
      // 连接失败
      cb(err);
      return;
    }

    // 先删除之前的可写事件监听
    _poller->RemoveEvent(sock);
    if (!attachEvent(sock, SocketNum::Sock_TCP)) {
      // 连接失败
      cb(SockException(Err_other, "add event to poller failed when connected"));
      return;
    }

    {
      LOCK_GUARD(_mtx_sock_fd);
      if (_sock_fd) {
        _sock_fd->SetConnected();
      }
    }
    // 连接成功
    cb(err);
  }

  bool Socket::attachEvent(int sock, SocketNum::SockType type) {
    std::weak_ptr<Socket> weak_self = shared_from_this();
    if (type == SocketNum::Sock_TCP_Server) {
      // tcp服务器
      auto result = _poller->AddEvent(sock, EventLoop::Event_Read | EventLoop::Event_Error,
                                      [weak_self, sock](int event) {
                                        if (auto strong_self = weak_self.lock()) {
                                          strong_self->onAccept(sock, event);
                                        }
                                      });
      return -1 != result;
    }

    // tcp客户端或udp
    auto read_buffer = _poller->getSharedBuffer();
    auto result = _poller->AddEvent(sock, EventLoop::Event_Read | EventLoop::Event_Error | EventLoop::Event_Write,
                                    [weak_self, sock, type, read_buffer](int event) {
                                      auto strong_self = weak_self.lock();
                                      if (!strong_self) {
                                        return;
                                      }

                                      if (event & EventLoop::Event_Read) {
                                        strong_self->onRead(sock, type, read_buffer);
                                      }
                                      if (event & EventLoop::Event_Write) {
                                        strong_self->onWriteAble(sock, type);
                                      }
                                      if (event & EventLoop::Event_Error) {
                                        strong_self->emitErr(GetSockErr(sock));
                                      }
                                    });

    return -1 != result;
  }

  ssize_t Socket::onRead(int sock_fd, SocketNum::SockType type, const BufferRaw::Ptr &buffer) noexcept {
    ssize_t ret = 0, nread = 0;
    auto data = buffer->Data();
    // 最后一个字节设置为'\0'
    auto capacity = buffer->GetCapacity() - 1;

    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    while (_enable_recv) {
      do {
        nread = recvfrom(sock_fd, data, capacity, 0, (struct sockaddr *) &addr, &len);
      } while (-1 == nread && RD_EINTR == GetError(true));

      if (nread == 0) {
        if (type == SocketNum::Sock_TCP) {
          emitErr(SockException(Err_eof, "end of file"));
        } else {
          LOG_WARN << "Recv eof on udp socket[" << sock_fd << "]";
        }
        return ret;
      }

      if (nread == -1) {
        auto err = GetError(true);
        if (err != RD_EAGAIN) {
          if (type == SocketNum::Sock_TCP) {
            emitErr(ToSockException(err));
          } else {
            LOG_WARN << "Recv err on udp socket[" << sock_fd << "]: " << strerror(err);
          }
        }
        return ret;
      }

      if (_enable_speed) {
        // 更新接收速率
        _recv_speed += nread;
      }

      ret += nread;
      data[nread] = '\0';
      // 设置buffer有效数据大小
      buffer->SetSize(nread);

      // 触发回调
      LOCK_GUARD(_mtx_event);
      try {
        // 此处捕获异常，目的是防止数据未读尽，epoll边沿触发失效的问题
        _on_read(buffer, (struct sockaddr *) &addr, len);
      } catch (std::exception &ex) {
        LOG_ERROR << "Exception occurred when emit on_read: " << ex.what();
      }
    }
    return 0;
  }

  bool Socket::emitErr(const SockException &err) noexcept {
    if (_err_emit) {
      return true;
    }
    _err_emit = true;
    std::weak_ptr<Socket> weak_self = shared_from_this();
    _poller->Async([weak_self, err]() {
      auto strong_self = weak_self.lock();
      if (!strong_self) {
        return;
      }
      LOCK_GUARD(strong_self->_mtx_event);
      try {
        strong_self->_on_err(err);
      } catch (std::exception &ex) {
        LOG_ERROR << "Exception occurred when emit on_err: " << ex.what();
      }
      // 延后关闭socket，只移除其io事件，防止Session对象析构时获取fd相关信息失败
      strong_self->closeSock(false);
    });
    return true;
  }

  ssize_t Socket::send(const char *buf, size_t size, struct sockaddr *addr, socklen_t addr_len, bool try_flush) {
    if (size <= 0) {
      size = strlen(buf);
      if (!size) {
        return 0;
      }
    }
    auto ptr = BufferRaw::Create();
    ptr->Assign(buf, size);
    return send(std::move(ptr), addr, addr_len, try_flush);
  }

  ssize_t Socket::send(string buf, struct sockaddr *addr, socklen_t addr_len, bool try_flush) {
    return send(std::make_shared<BufferString>(std::move(buf)), addr, addr_len, try_flush);
  }

  ssize_t Socket::send(ABuffer::Ptr buf, struct sockaddr *addr, socklen_t addr_len, bool try_flush) {
    if (!addr) {
      if (!_udp_send_dst) {
        return send_l(std::move(buf), false, try_flush);
      }
      // 本次发送未指定目标地址，但是目标定制已通过bindPeerAddr指定
      addr = (struct sockaddr *) _udp_send_dst.get();
      addr_len = SocketHelper::GetSockLen(addr);
    }
    return send_l(std::make_shared<BufferSocket>(std::move(buf), addr, addr_len), true, try_flush);
  }

  ssize_t Socket::send_l(ABuffer::Ptr buf, bool is_buf_sock, bool try_flush) {
    auto size = buf ? buf->Size() : 0;
    if (!size) {
      return 0;
    }

    {
      LOCK_GUARD(_mtx_send_buf_waiting);
      _send_buf_waiting.emplace_back(std::move(buf), is_buf_sock);
    }

    if (try_flush) {
      if (flushAll()) {
        return -1;
      }
    }

    return size;
  }

  int Socket::flushAll() {
    LOCK_GUARD(_mtx_sock_fd);

    if (!_sock_fd) {
      // 如果已断开连接或者发送超时
      return -1;
    }
    if (_sendable) {
      // 该socket可写
      return flushData(_sock_fd->RawFd(), _sock_fd->GetType(), false) ? 0 : -1;
    }

    // 该socket不可写,判断发送超时
    if (_send_flush_ticker.ElapsedTime() > _max_send_buffer_ms) {
      // 如果发送列队中最老的数据距今超过超时时间限制，那么就断开socket连接
      emitErr(SockException(Err_other, "socket send timeout"));
      return -1;
    }
    return 0;
  }

  void Socket::onFlushed() {
    bool flag;
    {
      LOCK_GUARD(_mtx_event);
      flag = _on_flush();
    }
    if (!flag) {
      setOnFlush(nullptr);
    }
  }

  void Socket::closeSock(bool close_fd) {
    _sendable = true;
    _enable_recv = true;
    _enable_speed = false;
    _con_timer = nullptr;
    _async_con_cb = nullptr;
    _send_flush_ticker.ResetTime();

    {
      LOCK_GUARD(_mtx_send_buf_waiting);
      _send_buf_waiting.clear();
    }

    {
      LOCK_GUARD(_mtx_send_buf_sending);
      _send_buf_sending.clear();
    }

    {
      LOCK_GUARD(_mtx_sock_fd);
      if (close_fd) {
        _err_emit = false;
        _sock_fd = nullptr;
      } else if (_sock_fd) {
        _sock_fd->DelEvent();
      }
    }
  }

  size_t Socket::getSendBufferCount() {
    size_t ret = 0;
    {
      LOCK_GUARD(_mtx_send_buf_waiting);
      ret += _send_buf_waiting.size();
    }

    {
      LOCK_GUARD(_mtx_send_buf_sending);
      _send_buf_sending.For_Each([&](BufferList::Ptr &buf) { ret += buf->Count(); });
    }
    return ret;
  }

  uint64_t Socket::elapsedTimeAfterFlushed() {
    return _send_flush_ticker.ElapsedTime();
  }

  int Socket::getRecvSpeed() {
    _enable_speed = true;
    return _recv_speed.getSpeed();
  }

  int Socket::getSendSpeed() {
    _enable_speed = true;
    return _send_speed.getSpeed();
  }

  bool Socket::listen(uint16_t port, const string &local_ip, int backlog) {
    closeSock();
    int fd = SocketHelper::Listen(port, local_ip.data(), backlog);
    if (fd == -1) {
      return false;
    }
    return fromSock_l(makeSock(fd, SocketNum::Sock_TCP_Server));
  }


  bool Socket::fromSock_l(SocketFD::Ptr fd) {
    if (!attachEvent(fd->RawFd(), fd->GetType())) {
      return false;
    }
    setSock(std::move(fd));
    return true;
  }

  int Socket::onAccept(int sock, int event) noexcept {
    int fd;
    struct sockaddr_storage peer_addr;
    socklen_t addr_len = sizeof(peer_addr);
    while (true) {
      if (event & EventLoop::Event_Read) {
        do {
          fd = (int) accept(sock, (struct sockaddr *) &peer_addr, &addr_len);
        } while (-1 == fd && RD_EINTR == GetError(true));

        if (fd == -1) {
          int err = GetError(true);
          if (err == RD_EAGAIN) {
            // 没有新连接
            return 0;
          }
          auto ex = ToSockException(err);
          LOG_ERROR << "Accept socket failed: " << ex.what();
          return -1;
        }

        SocketHelper::SetNoSigpipe(fd);
        SocketHelper::SetNoBlocked(fd);
        SocketHelper::SetNoDelay(fd);
        SocketHelper::SetSendBuf(fd);
        SocketHelper::SetRecvBuf(fd);
        SocketHelper::SetCloseWait(fd);
        SocketHelper::SetCloExec(fd);

        Socket::Ptr peer_sock;
        try {
          // 此处捕获异常，目的是防止socket未accept尽，epoll边沿触发失效的问题
          LOCK_GUARD(_mtx_event);
          // 拦截Socket对象的构造
          peer_sock = _on_before_accept(_poller);
        } catch (std::exception &ex) {
          LOG_ERROR << "Exception occurred when emit on_before_accept: " << ex.what();
          close(fd);
          continue;
        }

        if (!peer_sock) {
          // 此处是默认构造行为，也就是子Socket共用父Socket的poll线程并且关闭互斥锁
          peer_sock = Socket::CreateSocket(_poller, false);
        }

        // 设置好fd,以备在onAccept事件中可以正常访问该fd
        peer_sock->setSock(peer_sock->makeSock(fd, SocketNum::Sock_TCP));
        // 赋值peer ip，防止在执行setSock时，fd已经被reset断开
        memcpy(&peer_sock->_peer_addr, &peer_addr, addr_len);

        std::shared_ptr<void> completed(nullptr, [peer_sock, fd](void *) {
          try {
            // 然后把该fd加入poll监听(确保先触发onAccept事件然后再触发onRead等事件)
            if (!peer_sock->attachEvent(fd, SocketNum::Sock_TCP)) {
              // 加入poll监听失败，触发onErr事件，通知该Socket无效
              peer_sock->emitErr(SockException(Err_eof, "add event to poller failed when accept a socket"));
            }
          } catch (std::exception &ex) {
            LOG_ERROR << "Exception occurred: " << ex.what();
          }
        });

        try {
          // 此处捕获异常，目的是防止socket未accept尽，epoll边沿触发失效的问题
          LOCK_GUARD(_mtx_event);
          // 先触发onAccept事件，此时应该监听该Socket的onRead等事件
          _on_accept(peer_sock, completed);
        } catch (std::exception &ex) {
          LOG_ERROR << "Exception occurred when emit on_accept: " << ex.what();
          continue;
        }
      }

      if (event & EventLoop::Event_Error) {
        auto ex = GetSockErr(sock);
        emitErr(ex);
        LOG_ERROR << "TCP listener occurred a err: " << ex.what();
        return -1;
      }
    }
  }

  void Socket::setSock(SocketFD::Ptr fd) {
    LOCK_GUARD(_mtx_sock_fd);
    _sock_fd = std::move(fd);
    assert(_poller == _sock_fd->GetPoller());
    SocketHelper::GetSockLocalAddr(_sock_fd->RawFd(), _local_addr);
    SocketHelper::GetSockPeerAddr(_sock_fd->RawFd(), _peer_addr);
  }

  string Socket::GetLocalIp() {
    LOCK_GUARD(_mtx_sock_fd);
    if (!_sock_fd) {
      return "";
    }
    return SocketHelper::InetNToa((struct sockaddr *) &_local_addr);
  }

  uint16_t Socket::GetLocalPort() {
    LOCK_GUARD(_mtx_sock_fd);
    if (!_sock_fd) {
      return 0;
    }
    return SocketHelper::InetPort((struct sockaddr *) &_local_addr);
  }

  string Socket::GetRemoteIp() {
    LOCK_GUARD(_mtx_sock_fd);
    if (!_sock_fd) {
      return "";
    }
    if (_udp_send_dst) {
      return SocketHelper::InetNToa((struct sockaddr *) _udp_send_dst.get());
    }
    return SocketHelper::InetNToa((struct sockaddr *) &_peer_addr);
  }

  uint16_t Socket::GetRemotePort() {
    LOCK_GUARD(_mtx_sock_fd);
    if (!_sock_fd) {
      return 0;
    }
    if (_udp_send_dst) {
      return SocketHelper::InetPort((struct sockaddr *) _udp_send_dst.get());
    }
    return SocketHelper::InetPort((struct sockaddr *) &_peer_addr);
  }

  string Socket::GetIdentifier() const {
    static string class_name = "Socket: ";
    return class_name + std::to_string(reinterpret_cast<uint64_t>(this));
  }

  bool Socket::flushData(int sock, SocketNum::SockType type, bool poller_thread) {
    decltype(_send_buf_sending) send_buf_sending_tmp;
    {
      // 转移出二级缓存
      LOCK_GUARD(_mtx_send_buf_sending);
      if (!_send_buf_sending.empty()) {
        send_buf_sending_tmp.swap(_send_buf_sending);
      }
    }

    if (send_buf_sending_tmp.empty()) {
      _send_flush_ticker.ResetTime();
      do {
        {
          // 二级发送缓存为空，那么我们接着消费一级缓存中的数据
          LOCK_GUARD(_mtx_send_buf_waiting);
          if (!_send_buf_waiting.empty()) {
            // 把一级缓中数数据放置到二级缓存中并清空
            LOCK_GUARD(_mtx_event);
            auto send_result = _enable_speed ? [this](const ABuffer::Ptr &buffer, bool send_success) {
              if (send_success) {
                //更新发送速率
                _send_speed += buffer->Size();
              }
              LOCK_GUARD(_mtx_event);
              if (_send_result) {
                _send_result(buffer, send_success);
              }
            } : _send_result;
            send_buf_sending_tmp.emplace_back(
              BufferList::Create(std::move(_send_buf_waiting), std::move(send_result), type == SocketNum::Sock_UDP));
            break;
          }
        }
        // 如果一级缓存也为空,那么说明所有数据均写入socket了
        if (poller_thread) {
          // poller线程触发该函数，那么该socket应该已经加入了可写事件的监听；
          // 那么在数据列队清空的情况下，我们需要关闭监听以免触发无意义的事件回调
          stopWriteAbleEvent(sock);
          onFlushed();
        }
        return true;
      } while (false);
    }

    while (!send_buf_sending_tmp.empty()) {
      auto &packet = send_buf_sending_tmp.front();
      auto n = packet->Send(sock, _sock_flags);
      if (n > 0) {
        // 全部或部分发送成功
        if (packet->IsEmpty()) {
          // 全部发送成功
          send_buf_sending_tmp.pop_front();
          continue;
        }
        // 部分发送成功
        if (!poller_thread) {
          // 如果该函数是poller线程触发的，那么该socket应该已经加入了可写事件的监听，所以我们不需要再次加入监听
          startWriteAbleEvent(sock);
        }
        break;
      }

      // 一个都没发送成功
      int err = GetError(true);
      if (err == RD_EAGAIN) {
        // 等待下一次发送
        if (!poller_thread) {
          // 如果该函数是poller线程触发的，那么该socket应该已经加入了可写事件的监听，所以我们不需要再次加入监听
          startWriteAbleEvent(sock);
        }
        break;
      }

      // 其他错误代码，发生异常
      if (type == SocketNum::Sock_UDP) {
        // udp发送异常，把数据丢弃
        send_buf_sending_tmp.pop_front();
        LOG_WARN << "Send udp socket[" << sock << "] failed, Data ignored: " << strerror(err);
        continue;
      }
      // tcp发送失败时，触发异常
      emitErr(ToSockException(err));
      return false;
    }

    // 回滚未发送完毕的数据
    if (!send_buf_sending_tmp.empty()) {
      // 有剩余数据
      LOCK_GUARD(_mtx_send_buf_sending);
      send_buf_sending_tmp.swap(_send_buf_sending);
      _send_buf_sending.Append(send_buf_sending_tmp);
      // 二级缓存未全部发送完毕，说明该socket不可写，直接返回
      return true;
    }

    // 二级缓存已经全部发送完毕，说明该socket还可写，我们尝试继续写
    // 如果是poller线程，我们尝试再次写一次(因为可能其他线程调用了send函数又有新数据了)
    return poller_thread ? flushData(sock, type, poller_thread) : true;
  }

  void Socket::onWriteAble(int sock, SocketNum::SockType type) {
    bool empty_waiting;
    bool empty_sending;
    {
      LOCK_GUARD(_mtx_send_buf_waiting);
      empty_waiting = _send_buf_waiting.empty();
    }

    {
      LOCK_GUARD(_mtx_send_buf_sending);
      empty_sending = _send_buf_sending.empty();
    }

    if (empty_waiting && empty_sending) {
      // 数据已经清空了，我们停止监听可写事件
      stopWriteAbleEvent(sock);
    } else {
      // socket可写，我们尝试发送剩余的数据
      flushData(sock, type, true);
    }
  }

  void Socket::startWriteAbleEvent(int sock) {
    // 开始监听socket可写事件
    _sendable = false;
    int flag = _enable_recv ? EventLoop::Event_Read : 0;
    _poller->modifyEvent(sock, flag | EventLoop::Event_Error | EventLoop::Event_Write);
  }

  void Socket::stopWriteAbleEvent(int sock) {
    // 停止监听socket可写事件
    _sendable = true;
    int flag = _enable_recv ? EventLoop::Event_Read : 0;
    _poller->modifyEvent(sock, flag | EventLoop::Event_Error);
  }

  void Socket::enableRecv(bool enabled) {
    if (_enable_recv == enabled) {
      return;
    }
    _enable_recv = enabled;
    int read_flag = _enable_recv ? EventLoop::Event_Read : 0;
    // 可写时，不监听可写事件
    int send_flag = _sendable ? 0 : EventLoop::Event_Write;
    _poller->modifyEvent(rawFD(), read_flag | send_flag | EventLoop::Event_Error);
  }

  SocketFD::Ptr Socket::makeSock(int sock, SocketNum::SockType type) {
    return std::make_shared<SocketFD>(sock, type, _poller);
  }

  int Socket::rawFD() const {
    LOCK_GUARD(_mtx_sock_fd);
    if (!_sock_fd) {
      return -1;
    }
    return _sock_fd->RawFd();
  }

  bool Socket::alive() const {
    LOCK_GUARD(_mtx_sock_fd);
    return _sock_fd && !_err_emit;
  }

  SocketNum::SockType Socket::sockType() const {
    LOCK_GUARD(_mtx_sock_fd);
    if (!_sock_fd) {
      return SocketNum::Sock_Invalid;
    }
    return _sock_fd->GetType();
  }

  void Socket::setSendTimeOutSecond(uint32_t second) {
    _max_send_buffer_ms = second * 1000;
  }

  bool Socket::isSocketBusy() const {
    return !_sendable.load();
  }

  const EventLoop::Ptr &Socket::getPoller() const {
    return _poller;
  }

  SocketFD::Ptr Socket::cloneSockFD(const Socket &other) {
    SocketFD::Ptr sock;
    {
      LOCK_GUARD(other._mtx_sock_fd);
      if (!other._sock_fd) {
        LOG_WARN << "sockfd of src socket is null";
        return nullptr;
      }
      sock = std::make_shared<SocketFD>(*(other._sock_fd), _poller);
    }
    return sock;
  }

  bool Socket::cloneSocket(const Socket &other) {
    closeSock();
    auto sock = cloneSockFD(other);
    if (!sock) {
      return false;
    }
    return fromSock_l(std::move(sock));
  }

  bool Socket::bindPeerAddr(const struct sockaddr *dst_addr, socklen_t addr_len, bool soft_bind) {
    LOCK_GUARD(_mtx_sock_fd);
    if (!_sock_fd) {
      return false;
    }
    if (_sock_fd->GetType() != SocketNum::Sock_UDP) {
      return false;
    }
    addr_len = addr_len ? addr_len : SocketHelper::GetSockLen(dst_addr);
    if (soft_bind) {
      // 软绑定，只保存地址
      _udp_send_dst = std::make_shared<struct sockaddr_storage>();
      memcpy(_udp_send_dst.get(), dst_addr, addr_len);
    } else {
      // 硬绑定后，取消软绑定，防止memcpy目标地址的性能损失
      _udp_send_dst = nullptr;
      if (-1 == ::connect(_sock_fd->RawFd(), dst_addr, addr_len)) {
        LOG_WARN << "Connect socket to peer GetAddress failed: " << SocketHelper::InetNToa(dst_addr);
        return false;
      }
      memcpy(&_peer_addr, dst_addr, addr_len);
    }
    return true;
  }

  void Socket::setSendFlags(int flags) {
    _sock_flags = flags;
  }

RD_NAMESPACE_END