/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_NET_SOCKET_H
#define RENDU_NET_SOCKET_H

#include "address/address_family.h"
#include "endpoint/ip_end_point.h"
#include "protocol_type.h"
#include "socket_type.h"

NET_NAMESPACE_BEGIN

class SocketAsyncEventArgs;

class Socket {
public:
  Socket(AddressFamily address_family, SocketType socket_type, ProtocolType protocol_type);
  ~Socket();

public:
  void Close();
  IPEndPoint GetRemoteEndPoint();
  void SetRemoteEndPoint(IPEndPoint remote_end_point);
  bool ConnectAsync(SocketAsyncEventArgs *e);
  void Bind(IPEndPoint localEP);
  void Listen(int backlog);
  bool ReceiveAsync(SocketAsyncEventArgs *e);
  bool SendAsync(SocketAsyncEventArgs *e);
  bool AcceptAsync(SocketAsyncEventArgs* e);
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
  void SetTcpNoDelay(bool on);

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
  AddressFamily m_address_family;
  SocketType m_socket_type;
  ProtocolType m_protocol_type;
  IPEndPoint m_remote_end_point;
  IPEndPoint m_local_end_point;
  int m_socket_fd;
};

NET_NAMESPACE_END

#endif//RENDU_NET_SOCKET_H
