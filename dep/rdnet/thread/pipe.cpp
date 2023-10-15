/*
* Created by boil on 2023/10/26.
*/

#include <sys/filio.h>
#include "pipe.h"
#include "event_poller_pool.h"
#include "socket_util.h"

RD_NAMESPACE_BEGIN

  Pipe::Pipe(const OnRead &cb, const EventPoller::Ptr &poller) {
    _poller = poller;
    if (!_poller) {
      _poller = EventPollerPool::Instance().getPoller();
    }
    _pipe = std::make_shared<PipeWrapper>();
    auto pipe = _pipe;
    _poller->AddEvent(_pipe->ReadFD(), EventPoller::Event_Read, [cb, pipe](int event) {
#if defined(_WIN32)
      unsigned long nread = 1024;
#else
      int nread = 1024;
#endif //defined(_WIN32)
      ioctl(pipe->ReadFD(), FIONREAD, &nread);
#if defined(_WIN32)
      std::shared_ptr<char> buf(new char[nread + 1], [](char *ptr) {delete[] ptr; });
        buf.get()[nread] = '\0';
        nread = pipe->read(buf.get(), nread + 1);
        if (cb) {
            cb(nread, buf.get());
        }
#else
      char buf[nread + 1];
      buf[nread] = '\0';
      nread = pipe->Read(buf, sizeof(buf));
      if (cb) {
        cb(nread, buf);
      }
#endif // defined(_WIN32)
    });
  }

  Pipe::~Pipe() {
    if (_pipe) {
      auto pipe = _pipe;
      _poller->RemoveEvent(pipe->ReadFD(), [pipe](bool success) {});
    }
  }

  void Pipe::send(const char *buf, int size) {
    _pipe->Write(buf, size);
  }


RD_NAMESPACE_END