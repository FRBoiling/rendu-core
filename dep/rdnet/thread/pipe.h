/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_PIPE_H
#define RENDU_PIPE_H

#include "pipe_wrapper.h"
#include "poller/event_poller.h"

RD_NAMESPACE_BEGIN

  class Pipe {
  public:
    using OnRead = std::function<void(int size, const char *buf)>;

    Pipe(const OnRead &cb = nullptr, const EventPoller::Ptr &poller = nullptr);

    ~Pipe();

    void send(const char *send, int size = 0);

  private:
    std::shared_ptr<PipeWrapper> _pipe;
    EventPoller::Ptr _poller;
  };

RD_NAMESPACE_END

#endif //RENDU_PIPE_H
