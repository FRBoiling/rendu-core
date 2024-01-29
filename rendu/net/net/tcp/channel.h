/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_NET_CHANNEL_H
#define RENDU_NET_CHANNEL_H

#include "net_define.h"
#include "../../../time/time/timestamp.h"

NET_NAMESPACE_BEGIN

  class EventLoop;

///
/// A selectable I/O channel.
///
/// This class doesn't own the file descriptor.
/// The file descriptor could be a socket,an eventfd, a timerfd, or a signalfd
///
  class Channel : NonCopyable {
  public:
    typedef std::function<void(Timestamp)> EventCallback;

    Channel(EventLoop *loop, int fd);

    ~Channel();

  public:
    void handleEvent(Timestamp receiveTime);

    void setReadCallback(EventCallback cb) { readCallback_ = std::move(cb); }

    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }

    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }

    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

  public:
    /// Tie this channel to the owner object managed by shared_ptr,
    /// prevent the owner object being destroyed in handleEvent.
    void tie(const std::shared_ptr<void> &);

    int fd() const { return fd_; }

    int events() const { return events_; }

    void set_revents(int revt) { revents_ = revt; } // used by pollers
    // int revents() const { return revents_; }
    bool isNoneEvent() const { return events_ == kNoneEvent; }

    void enableReading() {
      events_ |= kReadEvent;
      update();
    }

    void disableReading() {
      events_ &= ~kReadEvent;
      update();
    }

    void enableWriting() {
      events_ |= kWriteEvent;
      update();
    }

    void disableWriting() {
      events_ &= ~kWriteEvent;
      update();
    }

    void disableAll() {
      events_ = kNoneEvent;
      update();
    }

    bool isWriting() const { return events_ & kWriteEvent; }

    bool isReading() const { return events_ & kReadEvent; }

    // for Poller
    int index() { return index_; }

    void set_index(int idx) { index_ = idx; }

    // for debug
    std::string reventsToString() const;

    std::string eventsToString() const;

    void doNotLogHup() { logHup_ = false; }

    EventLoop *ownerLoop() { return loop_; }

    void remove();

  private:
    static std::string eventsToString(int fd, int ev);

    void update();

    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

  private:
    int fd_;
    int events_;
    int revents_; // it's the received event types of epoll or poll

    EventLoop *loop_;

    int index_; // used by Poller.
    bool logHup_;

    std::weak_ptr<void> tie_;
    bool tied_;
    bool eventHandling_;
    bool addedToLoop_;
    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
  };

NET_NAMESPACE_END

#endif //RENDU_NET_CHANNEL_H
