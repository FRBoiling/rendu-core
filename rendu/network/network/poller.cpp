/*
* Created by boil on 2023/11/2.
*/

#include "poller.h"
#include "channel.h"
#include "poller/kqueue_poller.h"

RD_NAMESPACE_BEGIN

  Poller::Poller(EventLoop *loop)
    : ownerLoop_(loop) {
  }

  Poller::~Poller() = default;

  bool Poller::hasChannel(Channel *channel) const {
    assertInLoopThread();
    ChannelMap::const_iterator it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
  }

  Poller *Poller::newDefaultPoller(EventLoop *loop) {
    return new KqueuePoller(loop);
  }

RD_NAMESPACE_END