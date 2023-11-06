/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_NET_POLLER_H
#define RENDU_NET_POLLER_H

#include <map>
#include <vector>

#include "common/utils/non_copyable.h"
#include "time/timestamp.h"
#include "net_channel.h"
#include "net_poll_event.h"

RD_NAMESPACE_BEGIN

  class NetEventLoop;

  class NetPoller : NonCopyable {
  public:

    NetPoller(NetEventLoop *loop);

    virtual ~NetPoller() = 0;

  public:

  public:
    virtual int Resize(int size) = 0;

    virtual Timestamp Poll(int timeoutMs, NetChannelList &activeChannels) = 0;

    virtual int32 AddEvent(SOCKET fd, int32 mask) = 0;

    virtual int32 DelEvent(SOCKET fd, int32 mask) = 0;

    virtual void FillActiveChannels(int32 numEvents, NetChannelList &activeChannels) = 0;

  protected:
    NetEventLoop *ownerLoop_;
    NetChannelMap _channelMap;
  };

RD_NAMESPACE_END

#endif //RENDU_NET_POLLER_H
