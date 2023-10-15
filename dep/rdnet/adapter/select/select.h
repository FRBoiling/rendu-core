///*
//* Created by boil on 2023/10/25.
//*/
//
//#ifndef RENDU_SELECT_H
//#define RENDU_SELECT_H
//
//#include <sys/select.h>
//#include "../event_poller.h"
//
//RD_NAMESPACE_BEGIN
//
//  typedef struct ApiState {
//    fd_set rfds, wfds;
//    /* We need to have a copy of the fd sets as it's not safe to reuse
//     * FD sets after select(). */
//    fd_set _rfds, _wfds;
//  } ApiState;
//
//  class Select : public IEventPoller {
//  public:
//    explicit Select(EventLoop *eventLoop);
//
//    ~Select() override;
//
//  public:
//    int Create(int set_size) override;
//
//    int Resize(int set_size) override;
//
//    void Free() override;
//
//    int AddEvent(int fd, int mask) override;
//
//    void DelEvent(int fd, int mask) override;
//
//    int Poll(timeval *timeout) override;
//  };
//
//RD_NAMESPACE_END
//#endif //RENDU_SELECT_H
