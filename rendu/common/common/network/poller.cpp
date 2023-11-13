/*
* Created by boil on 2023/11/2.
*/

#include "poller.h"
#include "channel.h"
#include "poller/kqueue_poller.h"
#include "logger/log.h"

COMMON_NAMESPACE_BEGIN

  const int kNew = -1;
  const int kAdded = 1;
  const int kDeleted = 2;

  Poller::Poller(EventLoop *loop)
    : ownerLoop_(loop) {
  }

  Poller::~Poller() = default;

  Poller *Poller::NewDefaultPoller(EventLoop *loop) {
    return new KqueuePoller(loop);
  }


  bool Poller::HasChannel(Channel *channel) const {
    assertInLoopThread();
    ChannelMap::const_iterator it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
  }

  void Poller::UpdateChannel(Channel *channel) {
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
      AddEvent(channel,channel->events());
    } else {
      // update existing one with EPOLL_CTL_MOD/DEL
      int fd = channel->fd();
      (void) fd;
      assert(channels_.find(fd) != channels_.end());
      assert(channels_[fd] == channel);
      assert(index == kAdded);
      if (channel->isNoneEvent()) {
        DelEvent(channel,channel->events());
        channel->set_index(kDeleted);
      }
    }
  }

  void Poller::RemoveChannel(Channel *channel) {
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
      DelEvent(channel,channel->events());
    }
    channel->set_index(kNew);
  }

RD_NAMESPACE_END