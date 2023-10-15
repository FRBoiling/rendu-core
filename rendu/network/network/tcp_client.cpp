/*
* Created by boil on 2023/11/6.
*/

#include "tcp_client.h"

#include "tcp_connection.h"

#include "logger/log.h"
#include "connector.h"
#include "event_loop.h"
#include "sockets/sock_ops.h"

RD_NAMESPACE_BEGIN

  namespace detail {

    void removeConnection(EventLoop *loop, const TcpConnectionPtr &conn) {
      loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }

    void removeConnector(const ConnectorPtr &connector) {
      //connector->
    }

  }  // namespace detail

  TcpClient::TcpClient(EventLoop *loop,
                       const IPEndPoint &serverAddr,
                       const string &nameArg)
    : loop_(CHECK_NOTNULL(loop)),
      connector_(new Connector(loop, serverAddr)),
      name_(nameArg),
      connectionCallback_(defaultConnectionCallback),
      messageCallback_(defaultMessageCallback),
      retry_(false),
      connect_(true),
      nextConnId_(1) {
    connector_->setNewConnectionCallback(
      std::bind(&TcpClient::newConnection, this, _1));
    // FIXME setConnectFailedCallback
    LOG_INFO << "TcpClient::TcpClient[" << name_
             << "] - connector " << get_pointer(connector_);
  }

  TcpClient::~TcpClient() {
    LOG_INFO << "TcpClient::~TcpClient[" << name_
             << "] - connector " << get_pointer(connector_);
    TcpConnectionPtr conn;
    bool unique = false;
    {
      MutexLockGuard lock(mutex_);
      unique = connection_.unique();
      conn = connection_;
    }
    if (conn) {
      assert(loop_ == conn->getLoop());
      // FIXME: not 100% safe, if we are in different thread
      CloseCallback cb = std::bind(&detail::removeConnection, loop_, _1);
      loop_->runInLoop(
        std::bind(&TcpConnection::setCloseCallback, conn, cb));
      if (unique) {
        conn->forceClose();
      }
    } else {
      connector_->stop();
      // FIXME: HACK
      loop_->runAfter(1, std::bind(&detail::removeConnector, connector_));
    }
  }

  void TcpClient::connect() {
    // FIXME: check state
    LOG_INFO << "TcpClient::connect[" << name_ << "] - connecting to "
             << connector_->serverAddress().ToString();
    connect_ = true;
    connector_->start();
  }

  void TcpClient::disconnect() {
    connect_ = false;

    {
      MutexLockGuard lock(mutex_);
      if (connection_) {
        connection_->shutdown();
      }
    }
  }

  void TcpClient::stop() {
    connect_ = false;
    connector_->stop();
  }

  void TcpClient::newConnection(int sockfd) {
    loop_->assertInLoopThread();
    IPEndPoint peerAddr = IPEndPoint::FromIpPort(SockOps::GetPeerIP(sockfd),SockOps::GetPeerPort(sockfd)).value();
    char buf[32];
    snprintf(buf, sizeof buf, ":%s#%d", peerAddr.ToString().c_str(), nextConnId_);
    ++nextConnId_;
    string connName = name_ + buf;

    IPEndPoint localAddr = IPEndPoint::FromIpPort(SockOps::GetLocalIP(sockfd),SockOps::GetLocalPort(sockfd)).value();

    // FIXME poll with zero timeout to double confirm the new connection
    // FIXME use make_shared if necessary
    TcpConnectionPtr conn(new TcpConnection(loop_,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(
      std::bind(&TcpClient::removeConnection, this, _1)); // FIXME: unsafe
    {
      MutexLockGuard lock(mutex_);
      connection_ = conn;
    }
    conn->connectEstablished();
  }

  void TcpClient::removeConnection(const TcpConnectionPtr &conn) {
    loop_->assertInLoopThread();
    assert(loop_ == conn->getLoop());

    {
      MutexLockGuard lock(mutex_);
      assert(connection_ == conn);
      connection_.reset();
    }

    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    if (retry_ && connect_) {
      LOG_INFO << "TcpClient::connect[" << name_ << "] - Reconnecting to "
               << connector_->serverAddress().ToString();
      connector_->restart();
    }
  }

RD_NAMESPACE_END