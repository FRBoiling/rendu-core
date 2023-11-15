/*
* Created by boil on 2023/11/2.
*/

#include "socket.h"
#include "sockets/sock_ops.h"

NET_NAMESPACE_BEGIN

  Socket::~Socket() {
    SockOps::Close(sockfd_);
  }

  bool Socket::getTcpInfo(struct tcp_info *tcpi) const {
//    socklen_t len = sizeof(*tcpi);
//    memZero(tcpi, len);
//    return getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
    return true;
  }

  bool Socket::getTcpInfoString(char *buf, int len) const {
//    struct tcp_info tcpi;
//    bool ok = getTcpInfo(&tcpi);
//    if (ok)
//    {
//      snprintf(buf, len, "unrecovered=%u "
//                         "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
//                         "lost=%u retrans=%u rtt=%u rttvar=%u "
//                         "sshthresh=%u cwnd=%u total_retrans=%u",
//               tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
//               tcpi.tcpi_rto,          // Retransmit timeout in usec
//               tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
//               tcpi.tcpi_snd_mss,
//               tcpi.tcpi_rcv_mss,
//               tcpi.tcpi_lost,         // Lost packets
//               tcpi.tcpi_retrans,      // Retransmitted packets out
//               tcpi.tcpi_rtt,          // Smoothed round trip time in usec
//               tcpi.tcpi_rttvar,       // Medium deviation
//               tcpi.tcpi_snd_ssthresh,
//               tcpi.tcpi_snd_cwnd,
//               tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
//    }
    return true;
  }

  void Socket::bindAddress(const IPEndPoint &addr) {
    auto ip = addr.GetAddress().ToString();
    auto port = addr.GetPort();
    auto sock_address = SockOps::MakeSockAddr(ip.c_str(), port);
    int ret = SockOps::BindSock(sockfd_, port, ip.c_str(), sock_address.ss_family);
    if (ret < 0) {
      LOG_CRITICAL << "SocketOps::bindAddress fail";
    }
  }

  void Socket::listen() {
    SockOps::Listen(sockfd_);
  }

  int Socket::accept(const IPEndPoint &peeraddr) {
//    struct  sockaddr_storage local_storage;
//    SockOps::GetSockLocalAddr(sockfd_,local_storage);
//    auto len = SockOps::GetSockLen(local_storage.ss_family);
    socklen_t len;
    struct  sockaddr_storage peer_storage;
    int connfd = SockOps::Accept(sockfd_, peer_storage,len);
    return connfd;
  }

  void Socket::shutdownWrite() {
    SockOps::ShutdownWrite(sockfd_);
  }

  void Socket::setTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
                 &optval, static_cast<socklen_t>(sizeof optval));
    // FIXME CHECK
  }

  void Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                 &optval, static_cast<socklen_t>(sizeof optval));
    // FIXME CHECK
  }

  void Socket::setReusePort(bool on) {
#ifdef SO_REUSEPORT
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                           &optval, static_cast<socklen_t>(sizeof optval));
    if (ret < 0 && on) {
      LOG_CRITICAL << "SO_REUSEPORT failed.";
    }
#else
    if (on)
    {
      LOG_ERROR << "SO_REUSEPORT is not supported.";
    }
#endif
  }

  void Socket::setKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
               &optval, static_cast<socklen_t>(sizeof optval));
    // FIXME CHECK
  }

NET_NAMESPACE_END