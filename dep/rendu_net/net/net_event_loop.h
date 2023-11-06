/*
* Created by boil on 2023/11/10.
*/

#ifndef RENDU_NET_EVENT_LOOP_H
#define RENDU_NET_EVENT_LOOP_H

#include "define.h"
#include "net_channel.h"
#include "net_poller.h"

RD_NAMESPACE_BEGIN

  class NetEventLoop {
  public:
    using Ptr = std::shared_ptr<NetEventLoop>;
  public:
    NetEventLoop();
    ~NetEventLoop();
  public:
    void Loop();

    void Quit();

    void UpdateChannel(NetChannel *channel);

    void AddChannel(NetChannel *channel);

    void RemoveChannel(NetChannel *channel);

  private:
    bool quit_;

    std::unique_ptr<NetPoller> _poller;
    Timestamp _pollReturnTime;

    NetChannelList _activeChannels;
    NetChannel* _currentActiveChannel;
  };



RD_NAMESPACE_END

#endif //RENDU_NET_EVENT_LOOP_H
