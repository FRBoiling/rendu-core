///*
//* Created by boil on 2023/10/25.
//*/
//
//#include <mach/mach.h>
//#include "net_fwd.h"
//#include "select.h"
//#include "poller/event_loop.h"
//#include "poller/file_event.h"
//
//RD_NAMESPACE_BEGIN
//
//  Select::Select(EventLoop *eventLoop) : IEventPoller(eventLoop) {}
//
//  Select::~Select() {
//
//  }
//
//  int Select::Create(int set_size) {
//    ApiState *state = new ApiState();
//    if (!state) return -1;
//    FD_ZERO(&state->rfds);
//    FD_ZERO(&state->wfds);
//    _event_loop->m_api_state = state;
//    return 0;
//  }
//
//  int Select::Resize(int set_size) {
//    /* Just ensure we have enough room in the fd_set type. */
//    if (set_size >= FD_SETSIZE) return -1;
//    return 0;
//  }
//
//  void Select::Free() {
//    delete (_event_loop->m_api_state);
//  }
//
//  int Select::AddEvent(int fd, int mask) {
//    ApiState *state = _event_loop->m_api_state;
//    if (mask & AE_READABLE) FD_SET(fd, &state->rfds);
//    if (mask & AE_WRITABLE) FD_SET(fd, &state->wfds);
//    return 0;
//  }
//
//  void Select::DelEvent(int fd, int mask) {
//    ApiState *state = _event_loop->m_api_state;
//    if (mask & AE_READABLE) FD_CLR(fd, &state->rfds);
//    if (mask & AE_WRITABLE) FD_CLR(fd, &state->wfds);
//  }
//
//  int Select::Poll(timeval *timeout) {
//    ApiState *state = _event_loop->m_api_state;
//    memcpy(&state->_rfds, &state->rfds, sizeof(fd_set));
//    memcpy(&state->_wfds, &state->wfds, sizeof(fd_set));
//    int retval = select(_event_loop->m_maxfd + 1, &state->_rfds, &state->_wfds, NULL, timeout);
//    if (retval < 0 && errno != EINTR) {
//      panic("aeApiPoll: select, %s", strerror(errno));
//      return retval;
//    }
//    if (retval == 0){
//      return retval;
//    }
//    int numevents = 0;
//    for (int j = 0; j <= _event_loop->m_maxfd; j++) {
//      int mask = 0;
//      FileEvent *fe = &_event_loop->m_events[j];
//
//      if (fe->m_mask == AE_NONE) continue;
//      if (fe->m_mask & AE_READABLE && FD_ISSET(j, &state->_rfds))
//        mask |= AE_READABLE;
//      if (fe->m_mask & AE_WRITABLE && FD_ISSET(j, &state->_wfds))
//        mask |= AE_WRITABLE;
//      _event_loop->m_fired[numevents].m_fd = j;
//      _event_loop->m_fired[numevents].m_mask = mask;
//      numevents++;
//    }
//    return numevents;
//  }
//
//
//RD_NAMESPACE_END
