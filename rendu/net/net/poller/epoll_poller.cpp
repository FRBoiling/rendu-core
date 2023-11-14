///*
//* Created by boil on 2023/11/3.
//*/
//
//#include "epoll_poller.h"
//
//COMMON_NAMESPACE_BEGIN
//
//  EpollPoller::EpollPoller(EventLoop *loop)
//    : Poller(loop),
//      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
//      events_(kInitEventListSize) {
//    if (epollfd_ < 0) {
//      LOG_SYSFATAL << "EpollPoller::EpollPoller";
//    }
//  }
//
//  EpollPoller::~EpollPoller() {
//    ::close(epollfd_);
//  }
//
//  Timestamp EpollPoller::poll(int timeoutMs, ChannelList *activeChannels) {
////    int retval, numEvents = 0;
////    LOG_TRACE << "fd total count " << channels_.size();
////    retval = ::epoll_wait(epollfd_,
////                                 &*events_.begin(),
////                                 static_cast<int>(events_.size()),
////                                 timeoutMs);
////    int savedErrno = errno;
//    Timestamp now(Timestamp::now());
////    if (retval > 0) {
////      numEvents = retval;
////      LOG_TRACE << numEvents << " events happened";
////      for (int i = 0; i < numEvents; ++i) {
////        struct epoll_event *e = &events_[i];
////        Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
////        int32 mask = 0;
////        if (e->events & EPOLLIN) mask |= AE_READABLE;
////        if (e->events & EPOLLOUT) mask |= AE_WRITABLE;
////        if (e->events & EPOLLERR) mask |= AE_WRITABLE|AE_READABLE;
////        if (e->events & EPOLLHUP) mask |= AE_WRITABLE|AE_READABLE;
////        channel->set_revents(mask);
////        activeChannels->push_back(channel);
////      }
////      assert(implicit_cast<size_t>(numEvents) <= events_.size());
////      if (implicit_cast<size_t>(numEvents) == events_.size()) {
////        events_.resize(events_.size() * 2);
////      }
////    } else if (retval == 0) {
////      LOG_TRACE << "nothing happened";
////    } else {
////      // error happens, log uncommon ones
////      if (savedErrno != EINTR) {
////        errno = savedErrno;
////        LOG_SYSERR << "EpollPoller::poll()";
////      }
////    }
//    return now;
//  }
//
//
//  void EpollPoller::updateChannel(Channel *channel) {
//    Poller::assertInLoopThread();
//    const int index = channel->index();
//    LOG_TRACE << "fd = " << channel->fd()  << " events = " << channel->events() << " index = " << index;
//    if (index == kNew || index == kDeleted) {
//      // a new one, add with EPOLL_CTL_ADD
//      int fd = channel->fd();
//      if (index == kNew) {
//        assert(channels_.find(fd) == channels_.end());
//        channels_[fd] = channel;
//      } else // index == kDeleted
//      {
//        assert(channels_.find(fd) != channels_.end());
//        assert(channels_[fd] == channel);
//      }
//
//      channel->set_index(kAdded);
//      update(EPOLL_CTL_ADD, channel);
//    } else {
//      // update existing one with EPOLL_CTL_MOD/DEL
//      int fd = channel->fd();
//      (void) fd;
//      assert(channels_.find(fd) != channels_.end());
//      assert(channels_[fd] == channel);
//      assert(index == kAdded);
//      if (channel->isNoneEvent()) {
//        update(EPOLL_CTL_DEL, channel);
//        channel->set_index(kDeleted);
//      } else {
//        update(EPOLL_CTL_MOD, channel);
//      }
//    }
//  }
//
//  void EpollPoller::removeChannel(Channel *channel) {
//    Poller::assertInLoopThread();
//    int fd = channel->fd();
//    LOG_TRACE << "fd = " << fd;
//    assert(channels_.find(fd) != channels_.end());
//    assert(channels_[fd] == channel);
//    assert(channel->isNoneEvent());
//    int index = channel->index();
//    assert(index == kAdded || index == kDeleted);
//    size_t n = channels_.erase(fd);
//    (void) n;
//    assert(n == 1);
//
//    if (index == kAdded) {
//      update(EPOLL_CTL_DEL, channel);
//    }
//    channel->set_index(kNew);
//  }
//
//  void EpollPoller::update(int operation, Channel *channel) {
//    struct epoll_event event = {0};
//    event.events = channel->events();
//    event.data.ptr = channel;
//    int fd = channel->fd();
//    LOG_TRACE << "epoll_ctl op = " << operationToString(operation)
//              << " fd = " << fd << " event = { " << channel->eventsToString() << " }";
//    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
//      if (operation == EPOLL_CTL_DEL) {
//        LOG_SYSERR << "epoll_ctl op =" << operationToString(operation) << " fd =" << fd;
//      } else {
//        LOG_SYSFATAL << "epoll_ctl op =" << operationToString(operation) << " fd =" << fd;
//      }
//    }
//  }
//
//  const char *EpollPoller::operationToString(int op) {
//    switch (op) {
//      case EPOLL_CTL_ADD:
//        return "ADD";
//      case EPOLL_CTL_DEL:
//        return "DEL";
//      case EPOLL_CTL_MOD:
//        return "MOD";
//      default:
//        assert(false && "ERROR op");
//        return "Unknown Operation";
//    }
//  }
//
//COMMON_NAMESPACE_END