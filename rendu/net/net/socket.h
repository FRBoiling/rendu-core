/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_COMMON_SOCKET_H
#define RENDU_COMMON_SOCKET_H

#include "endpoint/ip_end_point.h"

COMMON_NAMESPACE_BEGIN

  class Socket {
  public:
    explicit Socket(int sockfd)
      : sockfd_(sockfd) {}

    // Socket(Socket&&) // move constructor in C++11
    ~Socket();

    int fd() const { return sockfd_; }

    // return true if success.
    bool getTcpInfo(struct tcp_info *) const;

    bool getTcpInfoString(char *buf, int len) const;

    /// abort if address in use
    void bindAddress(const IPEndPoint &localaddr);

    /// abort if address in use
    void listen();

    /// On success, returns a non-negative integer that is
    /// a descriptor for the accepted socket, which has been
    /// set to non-blocking and close-on-exec. *peeraddr is assigned.
    /// On error, -1 is returned, and *peeraddr is untouched.
    int accept(const IPEndPoint &peeraddr);

    void shutdownWrite();

    ///
    /// Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm).
    ///
    void setTcpNoDelay(bool on);

    ///
    /// Enable/disable SO_REUSEADDR
    ///
    void setReuseAddr(bool on);

    ///
    /// Enable/disable SO_REUSEPORT
    ///
    void setReusePort(bool on);

    ///
    /// Enable/disable SO_KEEPALIVE
    ///
    void setKeepAlive(bool on);

  private:
    const int sockfd_;
  };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_SOCKET_H
