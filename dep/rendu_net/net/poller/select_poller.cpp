/*
* Created by boil on 2023/11/11.
*/

#include "select_poller.h"

RD_NAMESPACE_BEGIN


  int32 SelectPoller::AddEvent(SOCKET fd, int32 mask) {
    if (mask & NetPollEvent::Type::NPE_READ) FD_SET(fd, &read_fds);
    if (mask & NetPollEvent::Type::NPE_WRITE) FD_SET(fd, &writ_fds);
    return 0;
  }

  int32 SelectPoller::DelEvent(SOCKET fd, int32 mask) {
    if (mask & NetPollEvent::Type::NPE_READ) FD_CLR(fd, &read_fds);
    if (mask & NetPollEvent::Type::NPE_WRITE) FD_CLR(fd, &writ_fds);
    return 0;
  }

  Timestamp SelectPoller::Poll(int timeoutMs, NetChannelList &activeChannels) {
    int retval, numevents = 0;
    memcpy(&_read_fds, &read_fds, sizeof(fd_set));
    memcpy(&_writ_fds, &writ_fds, sizeof(fd_set));
    memcpy(&_error_fds, &error_fds, sizeof(fd_set));
    Timestamp now(Timestamp::now());
    struct timeval tv;
    tv.tv_sec = (decltype(tv.tv_sec)) (timeoutMs / 1000);
    tv.tv_usec = 1000 * (timeoutMs % 1000);
    retval = select(_max_fd + 1, &_read_fds, &_writ_fds, &_error_fds, &tv);
    if (retval > 0) {
      FillActiveChannels(_max_fd, activeChannels);
    } else if (retval == -1 && errno != EINTR) {
      LOG_CRITICAL << "aeApiPoll: select," << strerror(errno);
    }

    return now;
  }

  void SelectPoller::FillActiveChannels(int32 numEvents, NetChannelList &activeChannels) {
    for (int j = 0; j <= numEvents; j++) {
      int mask = 0;
      int32 fd = j;
      auto channel = _channelMap.find(fd)->second;
      //收集select事件类型
      for (auto &pr: channel->m_pollEventMap) {
        if (pr.first == NetPollEvent::Type::NPE_NONE) continue;
        if (pr.first & NetPollEvent::Type::NPE_READ && FD_ISSET(j, &_read_fds))
          mask |= NetPollEvent::Type::NPE_READ;
        if (pr.first & NetPollEvent::Type::NPE_WRITE && FD_ISSET(j, &_writ_fds))
          mask |= NetPollEvent::Type::NPE_WRITE;
        if (pr.first & NetPollEvent::Type::NPE_ERROR && FD_ISSET(j, &_error_fds))
          mask |= NetPollEvent::Type::NPE_ERROR;
      }
      if (mask > 0) {
        activeChannels.push_back(channel);
      }
    }
  }


RD_NAMESPACE_END