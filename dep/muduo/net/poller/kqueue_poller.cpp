/*
* Created by boil on 2023/11/3.
*/

#include "kqueue_poller.h"
#include "types.h"
#include "log.h"
#include "sockets/sock_ops.h"

RD_NAMESPACE_BEGIN
  const int kNew = -1;
  const int kAdded = 1;
  const int kDeleted = 2;

  KqueuePoller::KqueuePoller(EventLoop *loop)
    : Poller(loop),
      poller_fd_(kqueue()),
      events_(16) {
    if (poller_fd_ < 0) {
      //TODO:BOIL

    }
  }

  KqueuePoller::~KqueuePoller() {
    SockOps::Close(poller_fd_);
  }

  Timestamp KqueuePoller::poll(int timeoutMs, Poller::ChannelList *activeChannels) {
    int retval, numEvents = 0;

    int event_size = static_cast<int32>(events_.size());
    if (timeoutMs > 0) {
      struct timespec timeout;
      struct timespec ts;
      ts.tv_sec = timeoutMs / 1000; // 1 second
      ts.tv_nsec = (timeoutMs % 1000) * 1000000; // 500000 nanoseconds
      retval = kevent(poller_fd_, NULL, 0, &*events_.begin(), event_size, &timeout);
    } else {
      retval = kevent(poller_fd_, NULL, 0, &*events_.begin(), event_size, NULL);
    }
    int savedErrno = errno;
    Timestamp now(Timestamp::now());
    if (retval > 0) {
      for (int i = 0; i < retval; ++i) {
        struct kevent &e = events_[i];
        Channel *channel = static_cast<Channel *>(e.udata);
        int32 mask = 0;
        if (e.filter == EVFILT_READ) { mask = RD_READABLE; }
        else if (e.filter == EVFILT_WRITE) { mask = RD_WRITABLE; }
        if (mask) {
          channel->set_revents(mask);
          activeChannels->push_back(channel);
          numEvents++;
        }
        if (numEvents > 0) {
          //      LOG_TRACE << numEvents << " events happened";
          assert(implicit_cast<size_t>(numEvents) <= events_.size());
          if (implicit_cast<size_t>(numEvents) == events_.size()) {
            events_.resize(events_.size() * 2);
          }
        } else if (numEvents == 0) {
          LOG_TRACE << "nothing happened";
        }
      }
    } else {
      if (savedErrno != EINTR) {
        errno = savedErrno;
        LOG_SYSERR << "KqueuePoller::poll()";
      }
    }
    return now;
  }

  void KqueuePoller::updateChannel(Channel *channel) {
    Poller::assertInLoopThread();
    const int index = channel->index();
    LOG_TRACE << "fd = " << channel->fd() << " events = " << channel->events() << " index = " << index;
    if (index == kNew || index == kDeleted) {
      // a new one, add with EPOLL_CTL_ADD
      int fd = channel->fd();
      if (index == kNew) {
        assert(channels_.find(fd) == channels_.end());
        channels_[fd] = channel;
      } else // index == kDeleted
      {
        assert(channels_.find(fd) != channels_.end());
        assert(channels_[fd] == channel);
      }

      channel->set_index(kAdded);
      update(EV_ADD, channel);
    } else {
      // update existing one with EPOLL_CTL_MOD/DEL
      int fd = channel->fd();
      (void) fd;
      assert(channels_.find(fd) != channels_.end());
      assert(channels_[fd] == channel);
      assert(index == kAdded);
      if (channel->isNoneEvent()) {
        update(EV_DELETE, channel);
        channel->set_index(kDeleted);
      }
    }
  }

  void KqueuePoller::removeChannel(Channel *channel) {
    Poller::assertInLoopThread();
    int fd = channel->fd();
    LOG_TRACE << "fd = " << fd;
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    size_t n = channels_.erase(fd);
    (void) n;
    assert(n == 1);

    if (index == kAdded) {
      update(EV_DELETE, channel);
    }
    channel->set_index(kNew);
  }

  bool KqueuePoller::hasChannel(Channel *channel) const {
    return Poller::hasChannel(channel);
  }

  void KqueuePoller::update(int operation, Channel *channel) {
    struct kevent event = {0};;
    int fd = channel->fd();
    EV_SET(&event, fd, channel->events(), operation, 0, 0, channel);
    LOG_TRACE << "kqueue update op = " << operationToString(operation)
              << " fd = " << fd << " event = { " << channel->eventsToString() << " }";
    if (kevent(poller_fd_, &event, 1, NULL, 0, NULL) < 0) {
      if (operation == EV_DELETE) {
        LOG_SYSERR << "epoll_ctl op =" << operationToString(operation) << " fd =" << fd;
      } else {
        LOG_SYSFATAL << "epoll_ctl op =" << operationToString(operation) << " fd =" << fd;
      }
    }
  }

  const char *KqueuePoller::operationToString(int op) {
    switch (op) {
      case EV_ADD:
        return "ADD";
      case EV_DELETE:
        return "DEL";
      default:
        assert(false && "ERROR op");
        return "Unknown Operation";
    }
  }


RD_NAMESPACE_END