/*
* Created by boil on 2023/11/3.
*/

#include "tcp_server.h"
#include "tcp_connection.h"
#include "log.h"
#include "sockets/sock_ops.h"
#include "endpoint/ip_end_point.h"

RD_NAMESPACE_BEGIN

  TcpServer::TcpServer(EventLoop *loop,
                       const IPEndPoint &listenAddr,
                       const string &nameArg,
                       Option option)
    : loop_(CHECK_NOTNULL(loop)),
      ipPort_(listenAddr.ToString()),
      name_(nameArg),
      acceptor_(new Acceptor(loop, listenAddr, option == kReusePort)),
      threadPool_(new EventLoopThreadPool(loop, name_)),
      connectionCallback_(defaultConnectionCallback),
      messageCallback_(defaultMessageCallback),
      nextConnId_(1) {
    acceptor_->setNewConnectionCallback(
      std::bind(&TcpServer::newConnection, this, _1, _2));
  }

  TcpServer::~TcpServer() {
    loop_->assertInLoopThread();
    LOG_TRACE << "TcpServer::~TcpServer [" << name_ << "] destructing";

    for (auto &item: connections_) {
      TcpConnectionPtr conn(item.second);
      item.second.reset();
      conn->getLoop()->runInLoop(
        std::bind(&TcpConnection::connectDestroyed, conn));
    }
  }

  void TcpServer::setThreadNum(int numThreads) {
    assert(0 <= numThreads);
    threadPool_->setThreadNum(numThreads);
  }

  void TcpServer::start() {
    if (started_.getAndSet(1) == 0) {
      threadPool_->start(threadInitCallback_);

      assert(!acceptor_->listening());
      loop_->runInLoop(
        std::bind(&Acceptor::listen, get_pointer(acceptor_)));
    }
  }

  IPEndPoint localAddr();
  void TcpServer::newConnection(int sockfd, const IPEndPoint &peerAddr) {
    loop_->assertInLoopThread();
    EventLoop *ioLoop = threadPool_->getNextLoop();
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    ++nextConnId_;
    string connName = name_ + buf;

    LOG_INFO << "TcpServer::newConnection [" << name_
             << "] - new connection [" << connName
             << "] from " << peerAddr.ToString();
    IPEndPoint localAddr = IPEndPoint::FromIpPort(SockOps::GetLocalIP(sockfd),SockOps::GetLocalPort(sockfd)).value();
    // FIXME poll with zero timeout to double confirm the new connection
    // FIXME use make_shared if necessary
    TcpConnectionPtr conn(new TcpConnection(ioLoop,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(
      std::bind(&TcpServer::removeConnection, this, _1)); // FIXME: unsafe
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
  }

  void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
    // FIXME: unsafe
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
  }

  void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {
    loop_->assertInLoopThread();
    LOG_INFO << "TcpServer::removeConnectionInLoop [" << name_
             << "] - connection " << conn->name();
    size_t n = connections_.erase(conn->name());
    (void) n;
    assert(n == 1);
    EventLoop *ioLoop = conn->getLoop();
    ioLoop->queueInLoop(
      std::bind(&TcpConnection::connectDestroyed, conn));
  }

RD_NAMESPACE_END