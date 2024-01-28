/*
* Created by boil on 2023/10/30.
*/

#include "tcp_client.h"
#include "net/event_loop_pool.h"
#include "utils/once_token.h"

RD_NAMESPACE_BEGIN

  StatisticImp(TcpClient)

  TcpClient::TcpClient(const EventLoop::Ptr &poller){
    setOnCreateSocket(nullptr);
  }

  TcpClient::~TcpClient() {
  }

  void TcpClient::Shutdown(const SockException &ex) {
    _timer.reset();
  }

  bool TcpClient::alive() const {
    if (_timer) {
      //连接中或已连接
      return true;
    }
    //在websocket client(zlmediakit)相关代码中，
    //_timer一直为空，但是socket fd有效，alive状态也应该返回true
    return _socket && _socket->alive();
  }

  void TcpClient::setNetAdapter(const string &local_ip) {
    _net_adapter = local_ip;
  }

  void TcpClient::connect(const string &url, uint16_t port, float timeout_sec, uint16_t local_port) {
    std::weak_ptr<TcpClient> weak_self = std::static_pointer_cast<TcpClient>(shared_from_this());
    _timer = std::make_shared<Timer>(2.0f, [weak_self]() -> bool {
      auto strong_self = weak_self.lock();
      if (!strong_self) {
        return false;
      }
      strong_self->onManagerSession();
      return true;
    }, _poller);

    _socket = _on_create_socket(_poller);
    if (_socket) {
      _poller = _socket->getPoller();
    }
    //创建一个Session;这里实现创建不同的服务会话实例
    auto helper = _session_alloc(std::static_pointer_cast<TcpClient>(shared_from_this()), _socket);
    _session = helper->session();

    auto sock_ptr = _socket.get();
    sock_ptr->setOnErr([weak_self, sock_ptr](const SockException &ex) {
      auto strong_self = weak_self.lock();
      if (!strong_self) {
        return;
      }
      if (sock_ptr != strong_self->GetSocket().get()) {
        //已经重连socket，上次的socket的事件忽略掉
        return;
      }
      strong_self->_timer.reset();
      auto session = strong_self->GetSession();
      LOG_TRACE << session->GetIdentifier() << " on err: " << ex;
      session->onError(ex);
    });
    LOG_TRACE << GetSession()->GetIdentifier() << " start connect " << url << ":" << port;
    sock_ptr->connect(url, port, [weak_self](const SockException &err) {
      auto strong_self = weak_self.lock();
      if (strong_self) {
        strong_self->onSockConnect(err);
      }
    }, timeout_sec, _net_adapter, local_port);
  }

  void TcpClient::onManagerSession() {
    assert(_poller->isCurrentThread());

    OnceToken token([&]() {
      _is_on_manager = true;
    }, [&]() {
      _is_on_manager = false;
    });
    try {
      _session->onManager();
    } catch (std::exception &ex) {
      LOG_WARN << ex.what();
    }
  }

  void TcpClient::onSockConnect(const SockException &ex) {
    LOG_TRACE << _session->GetIdentifier() << " connect result: " << ex;
    if (ex) {
      //连接失败
      _timer.reset();
      _session->onConnect(ex);
      return;
    }

    auto sock_ptr = _socket.get();
    std::weak_ptr<TcpClient> weak_self = static_pointer_cast<TcpClient>(shared_from_this());
    sock_ptr->setOnFlush([weak_self, sock_ptr]() {
      auto strong_self = weak_self.lock();
      if (!strong_self) {
        return false;
      }
      if (sock_ptr != strong_self->GetSocket().get()) {
        //已经重连socket，上传socket的事件忽略掉
        return false;
      }
      strong_self->GetSession()->onFlush();
      return true;
    });

    sock_ptr->setOnRead([weak_self, sock_ptr](const ABuffer::Ptr &pBuf, struct sockaddr *, int) {
      auto strong_self = weak_self.lock();
      if (!strong_self) {
        return;
      }
      if (sock_ptr != strong_self->GetSocket().get()) {
        //已经重连socket，上传socket的事件忽略掉
        return;
      }
      try {
        strong_self->GetSession()->onRecv(pBuf);
      } catch (std::exception &ex) {
        strong_self->Shutdown(SockException(Err_other, ex.what()));
      }
    });

    _session->onConnect(ex);
  }

  void TcpClient::setOnCreateSocket(Socket::onCreateSocket cb) {
    if (cb) {
      _on_create_socket = std::move(cb);
    } else {
      _on_create_socket = [](const EventLoop::Ptr &poller) {
        return Socket::CreateSocket(poller, false);
      };
    }
  }

//  std::string TcpClient::getIdentifier() const {
//    if (_id.empty()) {
//      static std::atomic <uint64_t> s_index{0};
//      _id = demangle(typeid(*this).name()) + "-" + StringUtil::ToString(++s_index);
//    }
//    return _id;
//  }

RD_NAMESPACE_END