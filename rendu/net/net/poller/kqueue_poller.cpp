/*
* Created by boil on 2023/11/3.
*/

#include "kqueue_poller.h"
#include "base/utils/type_cast.h"
#include "sockets/sock_ops.h"

NET_NAMESPACE_BEGIN

#define EVENT_MASK_MALLOC_SIZE(sz) (((sz) + 3) / 4)
#define EVENT_MASK_OFFSET(fd) ((fd) % 4 * 2)
#define EVENT_MASK_ENCODE(fd, mask) (((mask) & 0x3) << EVENT_MASK_OFFSET(fd))

  int getEventMask(const char *eventsMask, int fd) {
    return (eventsMask[fd / 4] >> EVENT_MASK_OFFSET(fd)) & 0x3;
  }

  void addEventMask(char *eventsMask, int fd, int mask) {
    eventsMask[fd / 4] |= EVENT_MASK_ENCODE(fd, mask);
  }

  void resetEventMask(char *eventsMask, int fd) {
    eventsMask[fd / 4] &= ~EVENT_MASK_ENCODE(fd, 0x3);
  }

  KqueuePoller::KqueuePoller(EventLoop *loop)
    : Poller(loop),
      poller_fd_(-1),
      events_(nullptr),
      eventsMask_(nullptr) {
    poller_fd_ = kqueue();
    if (poller_fd_ == -1) {
      assert(0);
    }
    SockOps::SetCloExec(poller_fd_);
    events_ = (struct kevent *) malloc(sizeof(struct kevent) * 16);
    eventsMask_ = (char *) malloc(EVENT_MASK_MALLOC_SIZE(16));
    memset(eventsMask_, 0, EVENT_MASK_MALLOC_SIZE(16));
  }

  KqueuePoller::~KqueuePoller() {
    SockOps::Close(poller_fd_);
    free(events_);
    free(eventsMask_);
  }

  int KqueuePoller::Resize(int size) {
    events_ = (struct kevent *) malloc(sizeof(struct kevent) * size);
    eventsMask_ = (char *) realloc(eventsMask_, EVENT_MASK_MALLOC_SIZE(size));
    memset(eventsMask_, 0, EVENT_MASK_MALLOC_SIZE(size));
    return 0;
  }

  void KqueuePoller::AddEvent(Channel *channel, int mask) const {
    int fd = channel->fd();
    RD_TRACE("AddEvent fd = {} event = [{}]",fd,channel->eventsToString());
    struct kevent event;
    if (mask & RD_READABLE) {
      EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, channel);
      if (kevent(poller_fd_, &event, 1, nullptr, 0, nullptr) == -1) {
        RD_ERROR("kevent op = {} fd = {}",operationToString(EV_ADD),channel->fd());
      }
    }
    if (mask & RD_WRITABLE) {
      EV_SET(&event, fd, EVFILT_WRITE, EV_ADD, 0, 0, channel);
      if (kevent(poller_fd_, &event, 1, nullptr, 0, nullptr) == -1) {
        RD_ERROR("kevent op = {} fd = {}",operationToString(EV_ADD),channel->fd());
      }
    }
  }

  void KqueuePoller::DelEvent(Channel *channel, int mask) const {
    int fd = channel->fd();
    RD_TRACE("DelEvent fd = {} event = [{}]",fd,channel->eventsToString());
    struct kevent event;
    if (mask & RD_READABLE) {
      EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
      if (kevent(poller_fd_, &event, 1, nullptr, 0, nullptr) == -1) {
        RD_ERROR("kevent op = {} fd = {}",operationToString(EV_DELETE),channel->fd());
      }
    }
    if (mask & RD_WRITABLE) {
      EV_SET(&event, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
      if (kevent(poller_fd_, &event, 1, NULL, 0, NULL) == -1) {
        RD_ERROR("kevent op = {} fd = {}",operationToString(EV_DELETE),channel->fd());
      }
    }
  }

  Timestamp KqueuePoller::Poll(int timeoutMs, Poller::ChannelList *activeChannels) {
    int retval, numEvents = 0;

    if (timeoutMs > 0) {
      struct timespec timeout;
      timeout.tv_sec = timeoutMs / 1000; // 1 second
      timeout.tv_nsec = (timeoutMs % 1000) * 1000000; // 500000 nanoseconds
      retval = kevent(poller_fd_, NULL, 0, events_, 16, &timeout);
    } else {
      retval = kevent(poller_fd_, NULL, 0, events_, 16, NULL);
    }
    int savedErrno = errno;
    Timestamp now(Timestamp::now());
    if (retval > 0) {
      /* Normally we execute the read event first and then the write event.
       * When the barrier is set, we will do it reverse.
       *
       * However, under kqueue, read and write events would be separate
       * events, which would make it impossible to control the order of
       * reads and writes. So we store the event's mask we've got and merge
       * the same fd events later. */
      for (int i = 0; i < retval; i++) {
        struct kevent &event = events_[i];
        int fd = (int) event.ident;
        int mask = 0;

        if (event.filter == EVFILT_READ) mask = RD_READABLE;
        else if (event.filter == EVFILT_WRITE) mask = RD_WRITABLE;
        addEventMask(eventsMask_, fd, mask);
      }

      /* Re-traversal to merge read and write events, and set the fd's mask to
       * 0 so that events are not added again when the fd is encountered again. */
      numEvents = 0;
      for (int i = 0; i < retval; i++) {
        struct kevent &event = events_[i];
        int fd = (int) event.ident;
        int mask = getEventMask(eventsMask_, fd);
        if (mask) {
          resetEventMask(eventsMask_, fd);
          Channel *channel = static_cast<Channel *>(event.udata);
          channel->set_revents(mask);
          activeChannels->push_back(channel);
          numEvents++;
        }
      }
    } else if (retval == -1 && savedErrno != EINTR) {
      errno = savedErrno;
      RD_ERROR("aeApiPoll : kevent, {}",strerror(errno));
    }
    return now;
  }


  const char *KqueuePoller::operationToString(int op) {
    switch (op) {
      case EV_ADD:
        return "ADD";
      case EV_DELETE:
        return "DEL";
      default:
        assert(false && "ERROR op");
        return "Unknown Operation";
    }
  }


NET_NAMESPACE_END