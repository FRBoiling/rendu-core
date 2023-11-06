/*
* Created by boil on 2023/11/3.
*/

#ifndef RENDU_COMMON_KQUEUE_POLLER_H
#define RENDU_COMMON_KQUEUE_POLLER_H

#include "poller.h"
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

RD_NAMESPACE_BEGIN

  class KqueuePoller : public Poller {

  public:
    KqueuePoller(EventLoop *loop);

    ~KqueuePoller() override;

  public:
    int Resize(int size) override;

    void AddEvent(Channel *channel,int mask) const override;

    void DelEvent(Channel *channel,int mask) const override;

    Timestamp Poll(int timeoutMs, ChannelList *activeChannels) override;


  private:
    static const int kInitEventListSize = 16;

    static const char *operationToString(int op);

  private:
    int poller_fd_;
    struct kevent* events_;
    char *eventsMask_;
  };

RD_NAMESPACE_END

#endif //RENDU_COMMON_KQUEUE_POLLER_H
