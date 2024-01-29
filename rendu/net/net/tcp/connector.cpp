/*
* Created by boil on 2023/11/3.
*/

#include "connector.h"
#include "base/string/str_error.h"
#include "event_loop.h"
#include "sockets/sock_ops.h"

NET_NAMESPACE_BEGIN

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop *loop, const IPEndPoint &serverAddr)
    : loop_(loop),
      serverAddr_(serverAddr),
      connect_(false),
      state_(kDisconnected),
      retryDelayMs_(kInitRetryDelayMs) {
  RD_TRACE("Connector::Connector {}", Convert::ToString(this));
}

Connector::~Connector() {
  RD_TRACE("Connector::~Connector {}", Convert::ToString(this));
  assert(!channel_);
}

void Connector::start() {
  connect_ = true;
  loop_->RunInLoop(std::bind(&Connector::startInLoop, this)); // FIXME: unsafe
}

void Connector::startInLoop() {
  loop_->assertInLoopThread();
  assert(state_ == kDisconnected);
  if (connect_) {
    connect();
  } else {
    RD_WARN("do not connect");
  }
}

void Connector::stop() {
  connect_ = false;
  loop_->QueueInLoop(std::bind(&Connector::stopInLoop, this)); // FIXME: unsafe
  // FIXME: cancel timer
}

void Connector::stopInLoop() {
  loop_->assertInLoopThread();
  if (state_ == kConnecting) {
    setState(kDisconnected);
    int sockfd = removeAndResetChannel();
    retry(sockfd);
  }
}

void Connector::connect() {
  auto sockAddr = SockOps::MakeSockAddr(serverAddr_.GetAddress().ToString().c_str(), serverAddr_.GetPort());
  int sockfd = SockOps::CreateNonblockingOrDie(sockAddr.ss_family);
  int ret = SockOps::Connect(sockfd, sockAddr);
  int savedErrno = (ret == 0) ? 0 : errno;
  switch (savedErrno) {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN:connecting(sockfd);
      break;

    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:retry(sockfd);
      break;

    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:LOG_CRITICAL << "connect error in Connector::startInLoop " << savedErrno;
      SockOps::Close(sockfd);
      break;

    default:LOG_CRITICAL << "Unexpected error in Connector::startInLoop " << savedErrno;
      SockOps::Close(sockfd);
      // connectErrorCallback_();
      break;
  }
}

void Connector::restart() {
  loop_->assertInLoopThread();
  setState(kDisconnected);
  retryDelayMs_ = kInitRetryDelayMs;
  connect_ = true;
  startInLoop();
}

void Connector::connecting(int sockfd) {
  setState(kConnecting);
  assert(!channel_);
  channel_.reset(new Channel(loop_, sockfd));
  channel_->setWriteCallback(
      std::bind(&Connector::handleWrite, this)); // FIXME: unsafe
  channel_->setErrorCallback(
      std::bind(&Connector::handleError, this)); // FIXME: unsafe

  // channel_->tie(shared_from_this()); is not working,
  // as channel_ is not managed by shared_ptr
  channel_->enableWriting();
}

int Connector::removeAndResetChannel() {
  channel_->disableAll();
  channel_->remove();
  int sockfd = channel_->fd();
  // Can't reset channel_ here, because we are inside Channel::handleEvent
  loop_->QueueInLoop(std::bind(&Connector::resetChannel, this)); // FIXME: unsafe
  return sockfd;
}

void Connector::resetChannel() {
  channel_.reset();
}

void Connector::handleWrite() {
  LOG_TRACE << "Connector::handleWrite " << state_;

  if (state_ == kConnecting) {
    int sockfd = removeAndResetChannel();
    int err = SockOps::GetSockError(sockfd);
    if (err) {
      LOG_WARN << "Connector::handleWrite - SO_ERROR = "
               << err << " " << strerror_tl(err);
      retry(sockfd);
    } else if (SockOps::IsSelfConnect(sockfd)) {
      LOG_WARN << "Connector::handleWrite - Self connect";
      retry(sockfd);
    } else {
      setState(kConnected);
      if (connect_) {
        newConnectionCallback_(sockfd);
      } else {
        SockOps::Close(sockfd);
      }
    }
  } else {
    // what happened?
    assert(state_ == kDisconnected);
  }
}

void Connector::handleError() {
  RD_ERROR("Connector::handleError state={}", state_);
  if (state_ == kConnecting) {
    int sockfd = removeAndResetChannel();
    int err = SockOps::GetSockError(sockfd);
    RD_ERROR("SO_ERROR = {} {}", err, strerror_tl(err));
    retry(sockfd);
  }
}

void Connector::retry(int sockfd) {
  SockOps::Close(sockfd);
  setState(kDisconnected);
  if (connect_) {
    RD_INFO("Connector::retry - Retry connecting to {} in {} milliseconds.", serverAddr_.ToString(), retryDelayMs_);
    loop_->RunAfter(retryDelayMs_ / 1000.0, std::bind(&Connector::startInLoop, shared_from_this()));
    retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
  } else {
    RD_DEBUG("do not connect");
  }
}

NET_NAMESPACE_END