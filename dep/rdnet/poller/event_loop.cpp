///*
//* Created by boil on 2023/10/24.
//*/
//
//#include "event_loop.h"
//#include "poller/cross_platform/select.h"
//#include "net_fwd.h"
//
//RD_NAMESPACE_BEGIN
//
//  EventLoop *EventLoop::Create(int setsize) {
//    m_poller = new Select(this);
//    m_poller->Create(setsize);
//    m_maxfd = -1;
//    m_fired = new FiredEvent[setsize];
//
//    m_events = new FileEvent[setsize];
//    for (int i = 0; i < setsize; i++)
//      m_events[i].m_mask = AE_NONE;
//    return this;
//  }
//
//  int EventLoop::Loop() {
//    struct timeval tv, *tvp = nullptr;
//    tv.tv_sec = tv.tv_usec = 10;
//    tvp = &tv;
//    int processed = 0;
//    int numevents = m_poller->Poll(tvp);
//    for (int j = 0; j < numevents; j++) {
//      int fd = m_fired[j].m_fd;
//      FileEvent *fe = &m_events[fd];
//      int mask = m_fired[j].m_mask;
//      int fired = 0;
//      int invert = fe->m_mask & AE_BARRIER;
//      if (!invert && fe->m_mask & mask & AE_READABLE) {
//        fe->rfileProc(fd, fe->clientData, mask);
//        fired++;
//        fe = &m_events[fd]; /* Refresh in case of resize. */
//      }
//      if (fe->m_mask & mask & AE_WRITABLE) {
//        if (!fired || fe->wfileProc != fe->rfileProc) {
//          fe->wfileProc(fd, fe->clientData, mask);
//          fired++;
//        }
//      }
//      if (invert) {
//        fe = &m_events[fd]; /* Refresh in case of resize. */
//        if ((fe->m_mask & mask & AE_READABLE) &&
//            (!fired || fe->wfileProc != fe->rfileProc)) {
//          fe->rfileProc(fd, fe->clientData, mask);
//          fired++;
//        }
//      }
//      processed++;
//    }
//    return processed;
//  }
//
//RD_NAMESPACE_END
