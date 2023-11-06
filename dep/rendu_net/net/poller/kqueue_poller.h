///*
//* Created by boil on 2023/11/3.
//*/
//
//#ifndef RENDU_KQUEUE_POLLER_H
//#define RENDU_KQUEUE_POLLER_H
//
//#include "net_poller.h"
//#include <sys/types.h>
//#include <sys/event.h>
//#include <sys/time.h>
//
//RD_NAMESPACE_BEGIN
//
//  class KqueuePoller : public NetPoller {
//  public:
//    KqueuePoller(NetEventLoop* loop);
//
//    ~KqueuePoller() override;
//
//  public:
//    int Resize(int size) override;
//
//    Timestamp Poll(int timeoutMs, NetChannelList &activeNetChannels) override;
//
//
//  private:
//    static const int kInitEventListSize = 16;
//
//    static const char *operationToString(int op);
//
//  private:
//    int poller_fd_;
//    struct kevent* events_;
//    char *eventsMask_;
//  };
//
//RD_NAMESPACE_END
//
//#endif //RENDU_KQUEUE_POLLER_H
