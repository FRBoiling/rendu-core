/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_POLLER_H
#define RENDU_POLLER_H

#include <map>
#include <vector>

#include "base/non_copyable.h"
#include "time/timestamp.h"
#include "event_loop.h"

RD_NAMESPACE_BEGIN

  class Channel;

  class Poller : NonCopyable {
  public:
    typedef std::vector<Channel *> ChannelList;

    Poller(EventLoop *loop);

    virtual ~Poller();

    /// Polls the I/O events.
    /// Must be called in the loop thread.
    virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0;

    /// Changes the interested I/O events.
    /// Must be called in the loop thread.
    virtual void updateChannel(Channel *channel) = 0;

    /// Remove the channel, when it destructs.
    /// Must be called in the loop thread.
    virtual void removeChannel(Channel *channel) = 0;

    virtual bool hasChannel(Channel *channel) const;

    static Poller *newDefaultPoller(EventLoop *loop);

    void assertInLoopThread() const {
      ownerLoop_->assertInLoopThread();
    }

  protected:
    typedef std::map<int, Channel *> ChannelMap;
    ChannelMap channels_;

  private:
    EventLoop *ownerLoop_;
  };

RD_NAMESPACE_END

#endif //RENDU_POLLER_H
