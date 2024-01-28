///*
//* Created by boil on 2023/11/3.
//*/
//
//#ifndef RENDU_NET_EPOLL_POLLER_H
//#define RENDU_NET_EPOLL_POLLER_H
//
//#include "poller.h"
//
//NET_NAMESPACE_BEGIN
//
//  class EpollPoller : public Poller {
//  public:
//    EpollPoller(EventLoop *loop);
//
//    ~EpollPoller() override;
//
//
//    Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;
//
//    void updateChannel(Channel *channel) override;
//
//    void removeChannel(Channel *channel) override;
//
//  private:
//    static const int kInitEventListSize = 16;
//
//    static const char *operationToString(int op);
//
//    void update(int operation, Channel *channel);
//
//  private:
//    int poller_fd_;
//    typedef std::vector<struct epoll_event> EventList;
//    EventList events_;
//  };
//
//NET_NAMESPACE_END
//
//#endif //RENDU_NET_EPOLL_POLLER_H
