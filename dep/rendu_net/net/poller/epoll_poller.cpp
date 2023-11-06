///*
//* Created by boil on 2023/11/11.
//*/
//
//#include "epoll_poller.h"
//#include "net_event_loop.h"
//
//RD_NAMESPACE_BEGIN
//
//  EpollPoller::EpollPoller(NetEventLoop *loop) : NetPoller(loop) {
//
//  }
//
//  EpollPoller::~EpollPoller() {
//
//  }
//
//  Timestamp EpollPoller::Poll(int timeoutMs, NetPoller::NetChannelList *activeChannels) {
//    return Timestamp();
//  }
//
//  void EpollPoller::UpdateChannel(NetChannel *channel) {
//
//  }
//
//  void EpollPoller::RemoveChannel(NetChannel *channel) {
//
//  }
//
//  void EpollPoller::fillActiveChannels(int numEvents, NetPoller::NetChannelList *activeChannels) const {
//
//  }
//
//
//RD_NAMESPACE_END