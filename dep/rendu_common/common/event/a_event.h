/*
* Created by boil on 2023/11/10.
*/

#ifndef RENDU_A_EVENT_H
#define RENDU_A_EVENT_H

#include "a_event_key.h"
#include "event_handler.h"

RD_NAMESPACE_BEGIN

////
/// This class doesn't own the file descriptor.
/// The file descriptor could be a socket,an eventfd, a timerfd, or a signalfd
///
  class AEvent {
  public:
    AEventKey key;
    EventHandler<AEvent> _event_handler;
  };

RD_NAMESPACE_END

#endif //RENDU_A_EVENT_H
