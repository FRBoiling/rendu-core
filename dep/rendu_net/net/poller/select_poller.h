/*
* Created by boil on 2023/11/11.
*/

#ifndef RENDU_SELECT_POLLER_H
#define RENDU_SELECT_POLLER_H

#include "net_poller.h"


RD_NAMESPACE_BEGIN

  class SelectPoller : public NetPoller {
  public:
    ~SelectPoller() override {};

    Timestamp Poll(int timeoutMs,NetChannelList & activeChannels) override;

    int32 AddEvent(SOCKET fd, int32 mask) override;

    int32 DelEvent(SOCKET fd, int32 mask) override;

    void FillActiveChannels(int32 numEvents, NetChannelList &activeChannels) override;

  private:
    int32 _max_fd;
    fd_set read_fds, writ_fds,error_fds;
    fd_set _read_fds, _writ_fds,_error_fds;
  };

RD_NAMESPACE_END

#endif //RENDU_SELECT_POLLER_H
