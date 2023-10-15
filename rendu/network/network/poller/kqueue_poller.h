/*
* Created by boil on 2023/11/3.
*/

#ifndef RENDU_KQUEUE_POLLER_H
#define RENDU_KQUEUE_POLLER_H

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
    Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;

    void updateChannel(Channel *channel) override;

    void removeChannel(Channel *channel) override;

    bool hasChannel(Channel *channel) const override;

    void AddEvent(Channel *channel) const;
    void DelEvent(Channel *channel) const;
  private:
    static const int kInitEventListSize = 16;

    static const char *operationToString(int op);

  private:
    int poller_fd_;
    typedef std::vector<struct kevent> EventList;
    EventList events_;
    char *eventsMask_;
  };

RD_NAMESPACE_END

#endif //RENDU_KQUEUE_POLLER_H
