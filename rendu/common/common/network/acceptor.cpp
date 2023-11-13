/*
* Created by boil on 2023/11/3.
*/
#include "acceptor.h"

#include "sockets/sock_ops.h"
#include "event_loop.h"
#include "log.h"

COMMON_NAMESPACE_BEGIN

  Acceptor::Acceptor(EventLoop *loop, const IPEndPoint &listenAddr, bool reuseport)
    : loop_(loop),
      acceptSocket_(SockOps::CreateNonblockingOrDie(SockOps::MakeSockAddr(listenAddr.GetAddress().ToString().c_str(),listenAddr.GetPort()).ss_family)),
      acceptChannel_(loop, acceptSocket_.fd()),
      listening_(false),
      idleFd_(::open(nullptr, O_RDONLY | O_CLOEXEC)) {
//    assert(idleFd_ >= 0);
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(reuseport);
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.setReadCallback(
      std::bind(&Acceptor::handleRead, this));
  }

  Acceptor::~Acceptor() {
    acceptChannel_.disableAll();
    acceptChannel_.remove();
    SockOps::Close(idleFd_);
  }

  void Acceptor::listen() {
    loop_->assertInLoopThread();
    listening_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
  }

  void Acceptor::handleRead() {
    loop_->assertInLoopThread();
    IPEndPoint peerAddr;
    //FIXME loop until no more
    int connfd = acceptSocket_.accept(peerAddr);
    if (connfd >= 0) {
      // string hostport = peerAddr.toIpPort();
      // LOG_TRACE << "Accepts of " << hostport;
      if (newConnectionCallback_) {
        newConnectionCallback_(connfd, peerAddr);
      } else {
        SockOps::Close(connfd);
      }
    } else {
      LOG_SYSERR << "in Acceptor::handleRead";
      // Read the section named "The special problem of
      // accept()ing when you can't" in libev's doc.
      // By Marc Lehmann, author of libev.
      if (errno == EMFILE) {
        SockOps::Close(idleFd_);
        idleFd_ = SockOps::Accept(acceptSocket_.fd());
        SockOps::Close(idleFd_);
        idleFd_ = open(nullptr, O_RDONLY | O_CLOEXEC);
      }
    }
  }

RD_NAMESPACE_END