/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_NET_POLLER_H
#define RENDU_NET_POLLER_H

#include <map>
#include <vector>

#include "base/utils/non_copyable.h"
#include "event_loop.h"
#include "../../../time/time/timestamp.h"

NET_NAMESPACE_BEGIN

  class Channel;

  class Poller : NonCopyable {
  public:
    typedef std::vector<Channel *> ChannelList;

    Poller(EventLoop *loop);

    virtual ~Poller();

  public:

    virtual void AddEvent(Channel *channel,int mask) const = 0;

    virtual void DelEvent(Channel *channel,int mask) const = 0;

    virtual int Resize(int size) = 0;

    /// Polls the I/O events.
    /// Must be called in the loop thread.
    virtual Timestamp Poll(int timeoutMs, ChannelList *activeChannels) = 0;


    /// Changes the interested I/O events.
    /// Must be called in the loop thread.
    virtual void UpdateChannel(Channel *channel) ;

    /// Remove the channel, when it destructs.
    /// Must be called in the loop thread.
    virtual void RemoveChannel(Channel *channel) ;

    virtual bool HasChannel(Channel *channel) const;



    static Poller *NewDefaultPoller(EventLoop *loop);

    void assertInLoopThread() const {
      ownerLoop_->assertInLoopThread();
    }

  protected:
    typedef std::map<int, Channel *> ChannelMap;
    ChannelMap channels_;

  protected:
    EventLoop *ownerLoop_;
  };

NET_NAMESPACE_END

#endif //RENDU_NET_POLLER_H
