/*
* Created by boil on 2023/11/2.
*/

#include "socket.h"
#include "sockets/sock_ops.h"
#include "socket_async_event_args.h"

NET_NAMESPACE_BEGIN

Socket::Socket(AddressFamily address_family, SocketType socket_type, ProtocolType protocol_type)
    : m_address_family(address_family), m_socket_type(socket_type), m_protocol_type(protocol_type), m_socket_fd(0) {
}

Socket::~Socket() {
  Close();
}

void Socket::Close() { SockOps::Close(m_socket_fd); }

IPEndPoint Socket::GetRemoteEndPoint() { return m_remote_end_point; }

void Socket::SetRemoteEndPoint(IPEndPoint remote_end_point) { m_remote_end_point = remote_end_point; }

bool Socket::ConnectAsync(SocketAsyncEventArgs *e) {
  return false;
}

void Socket::Listen(int backlog) {
}
void Socket::Bind(IPEndPoint localEP) {
}

bool ReceiveAsync(SocketAsyncEventArgs *e){
  return false;
}

bool SendAsync(SocketAsyncEventArgs *e){
  return false;
}

bool Socket::getTcpInfo(struct tcp_info *tcpi) const {
  //    socklen_t len = sizeof(*tcpi);
  //    memZero(tcpi, len);
  //    return getsockopt(m_socket_fd, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
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
  int ret = SockOps::BindSock(m_socket_fd, port, ip.c_str(), sock_address.ss_family);
  if (ret < 0) {
    RD_CRITICAL("SocketOps::bindAddress fail");
  }
}

void Socket::listen() {
  SockOps::Listen(m_socket_fd);
}

int Socket::accept(const IPEndPoint &peeraddr) {
  //    struct  sockaddr_storage local_storage;
  //    SockOps::GetSockLocalAddr(m_socket_fd,local_storage);
  //    auto len = SockOps::GetSockLen(local_storage.ss_family);
  socklen_t len;
  struct sockaddr_storage peer_storage;
  int connfd = SockOps::Accept(m_socket_fd, peer_storage, len);
  return connfd;
}

void Socket::shutdownWrite() {
  SockOps::ShutdownWrite(m_socket_fd);
}

void Socket::SetTcpNoDelay(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(m_socket_fd, IPPROTO_TCP, TCP_NODELAY,
               &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK
}

void Socket::setReuseAddr(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR,
               &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK
}

void Socket::setReusePort(bool on) {
#ifdef SO_REUSEPORT
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEPORT,
                         &optval, static_cast<socklen_t>(sizeof optval));
  if (ret < 0 && on) {
    RD_CRITICAL("SO_REUSEPORT fail");
  }
#else
  if (on) {
    LOG_ERROR << "SO_REUSEPORT is not supported.";
  }
#endif
}

void Socket::setKeepAlive(bool on) {
  int optval = on ? 1 : 0;
  setsockopt(m_socket_fd, SOL_SOCKET, SO_KEEPALIVE,
             &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK
}


NET_NAMESPACE_END