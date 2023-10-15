///*
//* Created by boil on 2023/10/24.
//*/
//
//#ifndef RENDU_KQUEUE_H
//#define RENDU_KQUEUE_H
//
//#include "define.h"
//#include "a_poller.h"
//#include <sys/types.h>
//#include <sys/event.h>
//#include <sys/time.h>
//
//RD_NAMESPACE_BEGIN
//
//  class KQueue : public APoller {
//
//    struct ApiState {
//      int kqfd;
//      struct kevent *events;
//
//      /* Events mask for merge read and write event.
//       * To reduce memory consumption, we use 2 bits to store the mask
//       * of an event, so that 1 byte will store the mask of 4 events. */
//      char *eventsMask;
//    };
//  public:
//
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
//
//  };
//
//RD_NAMESPACE_END
//
//#endif //RENDU_KQUEUE_H
