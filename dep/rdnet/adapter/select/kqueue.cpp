///*
//* Created by boil on 2023/10/24.
//*/
//
//#include "kqueue.h"
//
//RD_NAMESPACE_BEGIN
//
//  int KQueue::Create(int set_size) {
//    ApiState *state = new ApiState();
//    if (!state) return -1;
//    state->events = new struct kevent[set_size];
//    if (!state->events) {
//      delete(state);
//      return -1;
//    }
//    state->kqfd = kqueue();
//    if (state->kqfd == -1) {
//      delete(state->events);
//      delete(state);
//      return -1;
//    }
////    anetCloexec(state->kqfd);
////    state->eventsMask = zmalloc(EVENT_MASK_MALLOC_SIZE(_event_loop->set_size));
////    memset(state->eventsMask, 0, EVENT_MASK_MALLOC_SIZE(_event_loop->set_size));
////    eventLoop->apidata = state;
//    return 0;
//  }
//
//  int KQueue::Resize(int set_size) {
//    return 0;
//  }
//
//  void KQueue::Free() {
//  }
//
//  int KQueue::AddEvent(int fd, int mask) {
//    return 0;
//  }
//
//  void KQueue::DelEvent(int fd, int mask) {
//  }
//
//  int KQueue::Poll(timeval *timeout) {
//    return 0;
//  }
//
//RD_NAMESPACE_END