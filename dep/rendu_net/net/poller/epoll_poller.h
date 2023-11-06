///*
//* Created by boil on 2023/11/11.
//*/
//
//#ifndef RENDU_EPOLL_POLLER_H
//#define RENDU_EPOLL_POLLER_H
//
//#include "net_poller.h"
//#include <sys/epoll.h>
//
//RD_NAMESPACE_BEGIN
//
//  class EpollPoller : public NetPoller {
//  public:
//    EpollPoller(NetEventLoop *loop);
//
//    ~EpollPoller() override;
//
//  public:
//    Timestamp Poll(int timeoutMs, NetChannelList *activeChannels) override;
//
//    void UpdateChannel(NetChannel *channel) override;
//
//    void RemoveChannel(NetChannel *channel) override;
//
//  private:
//    void fillActiveChannels(int numEvents, NetChannelList *activeChannels) const;
//
//
//    typedef std::vector<struct epoll_event> EventList;
//
//    int epollfd_;
//    EventList events_;
//  };
//
//RD_NAMESPACE_END
//
//#endif //RENDU_EPOLL_POLLER_H
