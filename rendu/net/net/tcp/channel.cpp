/*
* Created by boil on 2023/11/2.
*/

#include "channel.h"
#include <sys/poll.h>
#include <sstream>

#include "poller.h"
#include "event_loop.h"

NET_NAMESPACE_BEGIN

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      logHup_(true),
      tied_(false),
      eventHandling_(false),
      addedToLoop_(false) {

}

Channel::~Channel() {
  assert(!eventHandling_);
  assert(!addedToLoop_);
  if (loop_->isInLoopThread()) {
    assert(!loop_->HasChannel(this));
  }
}

void Channel::tie(const std::shared_ptr<void> &obj) {
  tie_ = obj;
  tied_ = true;
}

void Channel::update() {
  addedToLoop_ = true;
  loop_->UpdateChannel(this);
}

void Channel::remove() {
  assert(isNoneEvent());
  addedToLoop_ = false;
  loop_->RemoveChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime) {
  std::shared_ptr<void> guard;
  if (tied_) {
    guard = tie_.lock();
    if (guard) {
      handleEventWithGuard(receiveTime);
    }
  } else {
    handleEventWithGuard(receiveTime);
  }
}

void Channel::handleEventWithGuard(Timestamp receiveTime) {
  eventHandling_ = true;
  RD_TRACE("{}", reventsToString());
  if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
    if (logHup_) {
      RD_WARN("fd ={} Channel::handle_event() POLLHUP",fd_);
    }
    if (closeCallback_) closeCallback_(receiveTime);
  }

  if (revents_ & POLLNVAL) {
    RD_WARN("fd ={} Channel::handle_event() POLLNVAL",fd_);
  }

  if (revents_ & (POLLERR | POLLNVAL)) {
    if (errorCallback_) errorCallback_(receiveTime);
  }

  if (revents_ & (POLLIN | POLLPRI)) {
    if (readCallback_) readCallback_(receiveTime);
  }
  if (revents_ & POLLOUT) {
    if (writeCallback_) writeCallback_(receiveTime);
  }
  eventHandling_ = false;
}

std::string Channel::reventsToString() const {
  return eventsToString(fd_, revents_);
}

std::string Channel::eventsToString() const {
  return eventsToString(fd_, events_);
}

std::string Channel::eventsToString(int fd, int ev) {
  std::ostringstream oss;
  oss << fd << ": ";
  if (ev & POLLIN)
    oss << "IN ";
  if (ev & POLLPRI)
    oss << "PRI ";
  if (ev & POLLOUT)
    oss << "OUT ";
  if (ev & POLLHUP)
    oss << "HUP ";
//    if (ev & POLLRDHUP)
//      oss << "RDHUP ";
  if (ev & POLLERR)
    oss << "ERR ";
  if (ev & POLLNVAL)
    oss << "NVAL ";

  return oss.str();
}

NET_NAMESPACE_END