/*
* Created by boil on 2023/11/3.
*/

#ifndef RENDU_NET_ACCEPTOR_H
#define RENDU_NET_ACCEPTOR_H

#include "socket.h"
#include "channel.h"

NET_NAMESPACE_BEGIN

  class EventLoop;

  class IPEndPoint;

///
/// Acceptor of incoming TCP connections.
///
  class Acceptor : NonCopyable {
  public:
    typedef std::function<void(int sockfd, const IPEndPoint &)> NewConnectionCallback;

    Acceptor(EventLoop *loop, const IPEndPoint &listenAddr, bool reuseport);

    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb) { newConnectionCallback_ = cb; }

    void listen();

    bool listening() const { return listening_; }

    // Deprecated, use the correct spelling one above.
    // Leave the wrong spelling here in case one needs to grep it for error messages.
    // bool listenning() const { return listening(); }

  private:
    void handleRead();

    EventLoop *loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listening_;
    int idleFd_;
  };

NET_NAMESPACE_END

#endif //RENDU_NET_ACCEPTOR_H
