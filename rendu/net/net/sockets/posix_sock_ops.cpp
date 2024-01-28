/*
* Created by boil on 2023/11/4.
*/


#if !defined(_WIN32) && !defined(__APPLE__)  //posix

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/un.h>
#include <poll.h>
#include <sys/uio.h>

#include <netdb.h>    /* for struct addrinfo/getaddrinfo() */
#include <netinet/in.h>  /* for sockaddr_in, in BSD at least */
#include <arpa/inet.h>

#include "log/log.h"
#include "dns_cache.h"
#include "errno/errno.h"

NET_NAMESPACE_BEGIN

  void Close(const int32 sfd) {
    close(sfd);
  }

  int BindSock6(int fd, const char *ifr_ip, uint16_t port) {
    SetIpv6Only(fd, false);
    struct sockaddr_in6 addr;
    bzero(&addr, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);
    if (1 != inet_pton(AF_INET6, ifr_ip, &(addr.sin6_addr))) {
      if (strcmp(ifr_ip, "0.0.0.0")) {
        LOG_WARN << "inet_pton to ipv6 GetAddress failed: " << ifr_ip;
      }
      addr.sin6_addr = IN6ADDR_ANY_INIT;
    }
    if (::bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
      LOG_WARN << "Bind socket failed: " << GetErrorMsg(true);
      return -1;
    }
    return 0;
  }

  int BindSock4(int fd, const char *ifr_ip, uint16_t port) {
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (1 != inet_pton(AF_INET, ifr_ip, &(addr.sin_addr))) {
      if (strcmp(ifr_ip, "::")) {
        LOG_WARN << "inet_pton to ipv4 GetAddress failed: " << ifr_ip;
      }
      addr.sin_addr.s_addr = INADDR_ANY;
    }
    if (::bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
      LOG_WARN << "Bind socket failed: " << GetErrorMsg(true);
      return -1;
    }
    return 0;
  }

  int BindSock(int fd, const char *ifr_ip, uint16_t port, int family) {
    switch (family) {
      case AF_INET:
        return BindSock4(fd, ifr_ip, port);
      case AF_INET6:
        return BindSock6(fd, ifr_ip, port);
      default:
        return -1;
    }
  }

  SOCKET Bind(const char *ifr_ip, uint16_t port, int family) {
    SOCKET sfd = socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (sfd == -1) {
      return sfd;
    }
    switch (family) {
      case AF_INET:
        return BindSock4(sfd, ifr_ip, port);
      case AF_INET6:
        return BindSock6(sfd, ifr_ip, port);
      default:
        return sfd;
    }
  }


  void Listen(const SOCKET sfd, int back_log /*= 1024*/) {
    //开始监听
    if (listen(sfd, back_log) == -1) {
//      LOG_WARN << "Listen socket failed: " << GetErrorMsg(true);
      Close(sfd);
    }
  }

  int Listen(const uint16_t port, const char *local_ip, int back_log) {
    int fd = -1;
    int family = SupportIPv6() ? (IsIPv4(local_ip) ? AF_INET : AF_INET6) : AF_INET;
    if ((fd = (int) socket(family, SOCK_STREAM, IPPROTO_TCP)) == -1) {
      LOG_WARN << "Create socket failed: " << GetErrorMsg(true);
      return -1;
    }

    SetReuseable(fd, true, false);
    SetNoBlocked(fd);
    SetCloExec(fd);

    if (BindSock(fd, local_ip, port, family) == -1) {
      close(fd);
      return -1;
    }

    //开始监听
    if (::listen(fd, back_log) == -1) {
      LOG_WARN << "Listen socket failed: " << GetErrorMsg(true);
      close(fd);
      return -1;
    }

    return fd;
  }


  SOCKET Accept(const SOCKET listen_sfd) {
    SOCKET sockfd = -1;
    struct sockaddr_storage storage;
    socklen_t addr_len = sizeof(storage);
    sockfd = accept(listen_sfd, (struct sockaddr *) &storage, &addr_len);
    return sockfd;
  }


  SOCKET Connect(const char *remote_host, uint16_t remote_port) {
    SOCKET sockfd = -1;
    struct sockaddr_storage storage;
    bzero(&storage, sizeof(storage));
    //先检查 host是ipv4
    struct in_addr addr;
    if (1 == inet_pton(AF_INET, remote_host, &addr)) {
      sockfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
      reinterpret_cast<struct sockaddr_in &>(storage).sin_addr = addr;
      reinterpret_cast<struct sockaddr_in &>(storage).sin_family = AF_INET;
      reinterpret_cast<struct sockaddr_in &>(storage).sin_port = htons(remote_port);
      if (connect(sockfd, (sockaddr *) &addr, GetSockLen((sockaddr *) &addr)) != 0) {
        return -1;
      }
      return sockfd;
    }
    //然后检查 host是ipv4
    struct in6_addr addr6;
    if (1 == inet_pton(AF_INET6, remote_host, &addr6)) {
      sockfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
      // host是ipv6
      reinterpret_cast<struct sockaddr_in6 &>(storage).sin6_addr = addr6;
      reinterpret_cast<struct sockaddr_in6 &>(storage).sin6_family = AF_INET6;
      reinterpret_cast<struct sockaddr_in6 &>(storage).sin6_port = htons(remote_port);
      if (connect(sockfd, (sockaddr *) &addr6, GetSockLen((sockaddr *) &addr6)) != 0) {
        return -1;
      }
      return sockfd;
    }
    throw std::invalid_argument(string("Not ip address: ") + remote_host);
  }

  int Connect(const char *host, uint16_t port, bool async, const char *local_ip, uint16_t local_port) {
    sockaddr_storage addr;
    //优先使用ipv4地址
    if (!GetDomainIP(host, port, addr, AF_INET, SOCK_STREAM, IPPROTO_TCP)) {
      //dns解析失败
      return -1;
    }

    int sockfd = (int) socket(addr.ss_family, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
      LOG_WARN << "Create socket failed: " << host;
      return -1;
    }

    SetReuseable(sockfd);
    SetNoSigpipe(sockfd);
    SetNoBlocked(sockfd, async);
    SetNoDelay(sockfd);
    SetSendBuf(sockfd);
    SetRecvBuf(sockfd);
    SetCloseWait(sockfd);
    SetCloExec(sockfd);

    if (BindSock(sockfd, local_ip, local_port, addr.ss_family) == -1) {
      close(sockfd);
      return -1;
    }

    if (::connect(sockfd, (sockaddr *) &addr, GetSockLen((sockaddr *) &addr)) == 0) {
      //同步连接成功
      return sockfd;
    }
    if (async && GetError(true) == RD_EAGAIN) {
      //异步连接成功
      return sockfd;
    }
    LOG_WARN << "Connect socket to " << host << " " << port << " failed: " << GetErrorMsg(true);
    close(sockfd);
    return -1;
  }


  bool IsIPv4(const char *host) {
    struct in_addr addr;
    return 1 == inet_pton(AF_INET, host, &addr);
  }

  bool IsIPv6(const char *host) {
    struct in6_addr addr;
    return 1 == inet_pton(AF_INET6, host, &addr);
  }

  bool IsIP(const char *str) {
    return IsIPv4(str) || IsIPv6(str);
  }

  bool SupportIPv6() {
    auto fd = socket(AF_INET6, S#include "sock_ops.h"
#include "apple_sock.h"
#include "log/log.h"
#include "dns_cache.h"
#include "errno/errno.h"

NET_NAMESPACE_BEGIN

  void Close(const int32 sfd) {
    close(sfd);
  }

  int BindSock6(int fd, const char *ifr_ip, uint16_t port) {
    SetIpv6Only(fd, false);
    struct sockaddr_in6 addr;
    bzero(&addr, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);
    if (1 != inet_pton(AF_INET6, ifr_ip, &(addr.sin6_addr))) {
      if (strcmp(ifr_ip, "0.0.0.0")) {
        LOG_WARN << "inet_pton to ipv6 GetAddress failed: " << ifr_ip;
      }
      addr.sin6_addr = IN6ADDR_ANY_INIT;
    }
    if (::bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
      LOG_WARN << "Bind socket failed: " << GetErrorMsg(true);
      return -1;
    }
    return 0;
  }

  int BindSock4(int fd, const char *ifr_ip, uint16_t port) {
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (1 != inet_pton(AF_INET, ifr_ip, &(addr.sin_addr))) {
      if (strcmp(ifr_ip, "::")) {
        LOG_WARN << "inet_pton to ipv4 GetAddress failed: " << ifr_ip;
      }
      addr.sin_addr.s_addr = INADDR_ANY;
    }
    if (::bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
      LOG_WARN << "Bind socket failed: " << GetErrorMsg(true);
      return -1;
    }
    return 0;
  }

  int BindSock(int fd, const char *ifr_ip, uint16_t port, int family) {
    switch (family) {
      case AF_INET:
        return BindSock4(fd, ifr_ip, port);
      case AF_INET6:
        return BindSock6(fd, ifr_ip, port);
      default:
        return -1;
    }
  }

  SOCKET Bind(const char *ifr_ip, uint16_t port, int family) {
    SOCKET sfd = socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (sfd == -1) {
      return sfd;
    }
    switch (family) {
      case AF_INET:
        return BindSock4(sfd, ifr_ip, port);
      case AF_INET6:
        return BindSock6(sfd, ifr_ip, port);
      default:
        return sfd;
    }
  }


  void Listen(const SOCKET sfd, int back_log /*= 1024*/) {
    //开始监听
    if (listen(sfd, back_log) == -1) {
//      LOG_WARN << "Listen socket failed: " << GetErrorMsg(true);
      Close(sfd);
    }
  }

  int Listen(const uint16_t port, const char *local_ip, int back_log) {
    int fd = -1;
    int family = SupportIPv6() ? (IsIPv4(local_ip) ? AF_INET : AF_INET6) : AF_INET;
    if ((fd = (int) socket(family, SOCK_STREAM, IPPROTO_TCP)) == -1) {
      LOG_WARN << "Create socket failed: " << GetErrorMsg(true);
      return -1;
    }

    SetReuseable(fd, true, false);
    SetNoBlocked(fd);
    SetCloExec(fd);

    if (BindSock(fd, local_ip, port, family) == -1) {
      close(fd);
      return -1;
    }

    //开始监听
    if (::listen(fd, back_log) == -1) {
      LOG_WARN << "Listen socket failed: " << GetErrorMsg(true);
      close(fd);
      return -1;
    }

    return fd;
  }


  SOCKET Accept(const SOCKET listen_sfd) {
    SOCKET sockfd = -1;
    struct sockaddr_storage storage;
    socklen_t addr_len = sizeof(storage);
    sockfd = accept(listen_sfd, (struct sockaddr *) &storage, &addr_len);
    return sockfd;
  }


  SOCKET Connect(const char *remote_host, uint16_t remote_port) {
    SOCKET sockfd = -1;
    struct sockaddr_storage storage;
    bzero(&storage, sizeof(storage));
    //先检查 host是ipv4
    struct in_addr addr;
    if (1 == inet_pton(AF_INET, remote_host, &addr)) {
      sockfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
      reinterpret_cast<struct sockaddr_in &>(storage).sin_addr = addr;
      reinterpret_cast<struct sockaddr_in &>(storage).sin_family = AF_INET;
      reinterpret_cast<struct sockaddr_in &>(storage).sin_port = htons(remote_port);
      if (connect(sockfd, (sockaddr *) &addr, GetSockLen((sockaddr *) &addr)) != 0) {
        return -1;
      }
      return sockfd;
    }
    //然后检查 host是ipv4
    struct in6_addr addr6;
    if (1 == inet_pton(AF_INET6, remote_host, &addr6)) {
      sockfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
      // host是ipv6
      reinterpret_cast<struct sockaddr_in6 &>(storage).sin6_addr = addr6;
      reinterpret_cast<struct sockaddr_in6 &>(storage).sin6_family = AF_INET6;
      reinterpret_cast<struct sockaddr_in6 &>(storage).sin6_port = htons(remote_port);
      if (connect(sockfd, (sockaddr *) &addr6, GetSockLen((sockaddr *) &addr6)) != 0) {
        return -1;
      }
      return sockfd;
    }
    throw std::invalid_argument(string("Not ip address: ") + remote_host);
  }

  int Connect(const char *host, uint16_t port, bool async, const char *local_ip, uint16_t local_port) {
    sockaddr_storage addr;
    //优先使用ipv4地址
    if (!GetDomainIP(host, port, addr, AF_INET, SOCK_STREAM, IPPROTO_TCP)) {
      //dns解析失败
      return -1;
    }

    int sockfd = (int) socket(addr.ss_family, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
      LOG_WARN << "Create socket failed: " << host;
      return -1;
    }

    SetReuseable(sockfd);
    SetNoSigpipe(sockfd);
    SetNoBlocked(sockfd, async);
    SetNoDelay(sockfd);
    SetSendBuf(sockfd);
    SetRecvBuf(sockfd);
    SetCloseWait(sockfd);
    SetCloExec(sockfd);

    if (BindSock(sockfd, local_ip, local_port, addr.ss_family) == -1) {
      close(sockfd);
      return -1;
    }

    if (::connect(sockfd, (sockaddr *) &addr, GetSockLen((sockaddr *) &addr)) == 0) {
      //同步连接成功
      return sockfd;
    }
    if (async && GetError(true) == RD_EAGAIN) {
      //异步连接成功
      return sockfd;
    }
    LOG_WARN << "Connect socket to " << host << " " << port << " failed: " << GetErrorMsg(true);
    close(sockfd);
    return -1;
  }


  bool IsIPv4(const char *host) {
    struct in_addr addr;
    return 1 == inet_pton(AF_INET, host, &addr);
  }

  bool IsIPv6(const char *host) {
    struct in6_addr addr;
    return 1 == inet_pton(AF_INET6, host, &addr);
  }

  bool IsIP(const char *str) {
    return IsIPv4(str) || IsIPv6(str);
  }

  bool SupportIPv6() {
    auto fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1) {
      return false;
    }
    Close(fd);
    return true;
  }


  socklen_t GetSockLen(const struct sockaddr *addr) {
    switch (addr->sa_family) {
      case AF_INET :
        return sizeof(sockaddr_in);
      case AF_INET6 :
        return sizeof(sockaddr_in6);
      default:
        assert(0);
        return 0;
    }
  }

  struct sockaddr_storage MakeSockAddr(const char *host, uint16_t port) {
    struct sockaddr_storage storage;
    bzero(&storage, sizeof(storage));

    struct in_addr addr;
    struct in6_addr addr6;
    if (1 == inet_pton(AF_INET, host, &addr)) {
      // host是ipv4
      reinterpret_cast<struct sockaddr_in &>(storage).sin_addr = addr;
      reinterpret_cast<struct sockaddr_in &>(storage).sin_family = AF_INET;
      reinterpret_cast<struct sockaddr_in &>(storage).sin_port = htons(port);
      return storage;
    }
    if (1 == inet_pton(AF_INET6, host, &addr6)) {
      // host是ipv6
      reinterpret_cast<struct sockaddr_in6 &>(storage).sin6_addr = addr6;
      reinterpret_cast<struct sockaddr_in6 &>(storage).sin6_family = AF_INET6;
      reinterpret_cast<struct sockaddr_in6 &>(storage).sin6_port = htons(port);
      return storage;
    }
    throw std::invalid_argument(string("Not ip GetAddress: ") + host);
  }


  int SetNoDelay(int fd, bool on) {
    return 0;
  }

  int SetNoSigpipe(int fd) {
#if defined(SO_NOSIGPIPE)
    int Set = 1;
    auto ret = setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (char *) &Set, sizeof(int));
    if (ret == -1) {
      LOG_TRACE << "setsockopt SO_NOSIGPIPE failed";
    }
    return ret;
#else
    return -1;
#endif
  }

  int SetNoBlocked(int fd, bool noblock) {
#if defined(_WIN32)
    unsigned long ul = noblock;
#else
    int ul = noblock;
#endif //defined(_WIN32)
    int ret = ioctl(fd, FIONBIO, &ul); //设置为非阻塞模式
    if (ret == -1) {
      LOG_TRACE << "ioctl FIONBIO failed";
    }

    return ret;
  }

  int SetCloExec(int fd, bool on) {
#if !defined(_WIN32)
    int flags = fcntl(fd, F_GETFD);
    if (flags == -1) {
      LOG_TRACE << "fcntl F_GETFD failed";
      return -1;
    }
    if (on) {
      flags |= FD_CLOEXEC;
    } else {
      int cloexec = FD_CLOEXEC;
      flags &= ~cloexec;
    }
    int ret = fcntl(fd, F_SETFD, flags);
    if (ret == -1) {
      LOG_TRACE << "fcntl F_SETFD failed";
      return -1;
    }
    return ret;
#else
    return -1;
#endif
  }


  int SetCloseWait(int fd, int second) {
    linger m_sLinger;
    //在调用closesocket()时还有数据未发送完，允许等待
    // 若m_sLinger.l_onoff=0;则调用closesocket()后强制关闭
    m_sLinger.l_onoff = (second > 0);
    m_sLinger.l_linger = second; //设置等待时间为x秒
    int ret = setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *) &m_sLinger, sizeof(linger));
    if (ret == -1) {
#ifndef _WIN32
      LOG_TRACE << "setsockopt SO_LINGER failed";
#endif
    }
    return ret;
  }

  int SetReuseable(int fd, bool on, bool reuse_port) {
    int opt = on ? 1 : 0;
    int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, static_cast<socklen_t>(sizeof(opt)));
    if (ret == -1) {
      LOG_TRACE << "setsockopt SO_REUSEADDR failed";
      return ret;
    }
#if defined(SO_REUSEPORT)
    if (reuse_port) {
      ret = setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (char *) &opt, static_cast<socklen_t>(sizeof(opt)));
      if (ret == -1) {
        LOG_TRACE << "setsockopt SO_REUSEPORT failed";
      }
    }
#endif
    return ret;
  }


  int SetKeepAlive(int fd, bool on, int interval, int idle, int times) {
    // Enable/disable the keep-alive option
    int opt = on ? 1 : 0;
    int ret = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &opt, static_cast<socklen_t>(sizeof(opt)));
    if (ret == -1) {
      LOG_TRACE << "setsockopt SO_KEEPALIVE failed";
    }
#if !defined(_WIN32)
#if !defined(SOL_TCP) && defined(IPPROTO_TCP)
#define SOL_TCP IPPROTO_TCP
#endif
#if !defined(TCP_KEEPIDLE) && defined(TCP_KEEPALIVE)
#define TCP_KEEPIDLE TCP_KEEPALIVE
#endif
    // Set the keep-alive parameters
    if (on && interval > 0 && ret != -1) {
      ret = setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, (char *) &idle, static_cast<socklen_t>(sizeof(idle)));
      if (ret == -1) {
        LOG_TRACE << "setsockopt TCP_KEEPIDLE failed";
      }
      ret = setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (char *) &interval, static_cast<socklen_t>(sizeof(interval)));
      if (ret == -1) {
        LOG_TRACE << "setsockopt TCP_KEEPINTVL failed";
      }
      ret = setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (char *) &times, static_cast<socklen_t>(sizeof(times)));
      if (ret == -1) {
        LOG_TRACE << "setsockopt TCP_KEEPCNT failed";
      }
    }
#endif
    return ret;
  }

  int SetBroadcast(int fd, bool on) {
    int opt = on ? 1 : 0;
    int ret = setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (char *) &opt, static_cast<socklen_t>(sizeof(opt)));
    if (ret == -1) {
      LOG_TRACE << "setsockopt SO_BROADCAST failed";
    }
    return ret;
  }


  int SetRecvBuf(int fd, int size) {
    int ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *) &size, sizeof(size));
    if (ret == -1) {
      LOG_TRACE << "setsockopt SO_RCVBUF failed";
    }
    return ret;
  }

  int SetSendBuf(int fd, int size) {
    int ret = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *) &size, sizeof(size));
    if (ret == -1) {
      LOG_TRACE << "setsockopt SO_SNDBUF failed";
    }
    return ret;
  }


  int SetIpv6Only(int fd, bool flag) {
    int opt = flag;
    int ret = setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, (char *) &opt, sizeof opt);
    if (ret == -1) {
      LOG_TRACE << "setsockopt IPV6_V6ONLY failed";
    }
    return ret;
  }

#if defined(_WIN32)
  static onceToken g_token([]() {
  WORD wVersionRequested = MAKEWORD(2, 2);
  WSADATA wsaData;
  WSAStartup(wVersionRequested, &wsaData);
}, []() {
  WSACleanup();
});
int ioctl(int fd, long cmd, u_long *ptr) {
  return ioctlsocket(fd, cmd, ptr);
}
int close(int fd) {
  return closesocket(fd);
}
#if (_WIN32_WINNT < _WIN32_WINNT_VISTA)
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size) {
  struct sockaddr_storage ss;
  unsigned long s = size;

  ZeroMemory(&ss, sizeof(ss));
  ss.ss_family = af;

  switch (af) {
  case AF_INET:
      ((struct sockaddr_in *)&ss)->sin_addr = *(struct in_addr *)src;
      break;
  case AF_INET6:
      ((struct sockaddr_in6 *)&ss)->sin6_addr = *(struct in6_addr *)src;
      break;
  default:
      return NULL;
  }
  /* cannot direclty use &size because of strict aliasing rules */
  return (WSAAddressToString((struct sockaddr *)&ss, sizeof(ss), NULL, dst, &s) == 0) ? dst : NULL;
}
int inet_pton(int af, const char *src, void *dst) {
  struct sockaddr_storage ss;
  int size = sizeof(ss);
  char src_copy[INET6_ADDRSTRLEN + 1];

  ZeroMemory(&ss, sizeof(ss));
  /* stupid non-const API */
  strncpy(src_copy, src, INET6_ADDRSTRLEN + 1);
  src_copy[INET6_ADDRSTRLEN] = 0;

  if (WSAStringToAddress(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0) {
      switch (af) {
      case AF_INET:
          *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
          return 1;
      case AF_INET6:
          *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
          return 1;
      }
  }
  return 0;
}
#endif
#endif // defined(_WIN32)

  string InetNTop(int af, const void *addr) {
    string ret;
    ret.resize(128);
    if (!inet_ntop(af, const_cast<void *>(addr), (char *) ret.data(), ret.size())) {
      ret.clear();
    } else {
      ret.resize(strlen(ret.data()));
    }
    return ret;
  }
  string InetNToa(const struct in_addr &addr) {
    return InetNTop(AF_INET, &addr);
  }
  std::string InetNToa(const struct in6_addr &addr) {
    return InetNTop(AF_INET6, &addr);
  }
  std::string InetNToa(const struct sockaddr *addr) {
    switch (addr->sa_family) {
      case AF_INET:
        return InetNToa(((struct sockaddr_in *) addr)->sin_addr);
      case AF_INET6: {
        if (IN6_IS_ADDR_V4MAPPED(&((struct sockaddr_in6 *) addr)->sin6_addr)) {
          struct in_addr addr4;
          memcpy(&addr4, 12 + (char *) &(((struct sockaddr_in6 *) addr)->sin6_addr), 4);
          return InetNToa(addr4);
        }
        return InetNToa(((struct sockaddr_in6 *) addr)->sin6_addr);
      }
      default:
        return "";
    }
  }

  uint16_t InetPort(const struct sockaddr *addr) {
    switch (addr->sa_family) {
      case AF_INET:
        return ntohs(((struct sockaddr_in *) addr)->sin_port);
      case AF_INET6:
        return ntohs(((struct sockaddr_in6 *) addr)->sin6_port);
      default:
        return 0;
    }
  }

  bool GetDomainIP(const char *host, uint16_t port, struct sockaddr_storage &addr,
                   int ai_family, int ai_socktype, int ai_protocol, int expire_sec) {
    bool flag = DnsCache::Instance().getDomainIP(host, addr, ai_family, ai_socktype, ai_protocol, expire_sec);
    if (flag) {
      switch (addr.ss_family) {
        case AF_INET :
          ((sockaddr_in *) &addr)->sin_port = htons(port);
          break;
        case AF_INET6 :
          ((sockaddr_in6 *) &addr)->sin6_port = htons(port);
          break;
        default:
          break;
      }
    }
    return flag;
  }



  int GetSockError(int fd) {
    int opt;
    socklen_t optLen = static_cast<socklen_t>(sizeof(opt));

    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *) &opt, &optLen) < 0) {
      return GetError(true);
    } else {
      return TranslatePosixError(opt);
    }
  }


  bool GetSockLocalAddr(int fd, struct sockaddr_storage &addr) {
    socklen_t addr_len = sizeof(addr);
    if (-1 == getsockname(fd, (struct sockaddr *) &addr, &addr_len)) {
      return false;
    }
    return true;
  }

  bool GetSockPeerAddr(int fd, struct sockaddr_storage &addr) {
    socklen_t addr_len = sizeof(addr);
    if (-1 == getpeername(fd, (struct sockaddr *) &addr, &addr_len)) {
      return false;
    }
    return true;
  }

  string GetLocalIP(int fd) {
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    if (-1 == getsockname(fd, (struct sockaddr *) &addr, &addr_len)) {
      return "";
    }
    return InetNToa((struct sockaddr *) &addr);
  }

  string GetPeerIP(int fd) {
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    if (-1 == getpeername(fd, (struct sockaddr *) &addr, &addr_len)) {
      return "";
    }
    return InetNToa((struct sockaddr *) &addr);
  }

  uint16_t GetLocalPort(int fd) {
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    if (-1 == getsockname(fd, (struct sockaddr *) &addr, &addr_len)) {
      return 0;
    }
    return InetPort((struct sockaddr *) &addr);
  }

  uint16_t GetPeerPort(int fd) {
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    if (-1 == getpeername(fd, (struct sockaddr *) &addr, &addr_len)) {
      return 0;
    }
    return InetPort((struct sockaddr *) &addr);
  }


  bool CheckIP(string &address, const string &ip) {
    if (ip != "127.0.0.1" && ip != "0.0.0.0") {
      /*获取一个有效IP*/
      address = ip;
      uint32_t addressInNetworkOrder = htonl(inet_addr(ip.data()));
      if (/*(addressInNetworkOrder >= 0x0A000000 && addressInNetworkOrder < 0x0E000000) ||*/
        (addressInNetworkOrder >= 0xAC100000 && addressInNetworkOrder < 0xAC200000) ||
        (addressInNetworkOrder >= 0xC0A80000 && addressInNetworkOrder < 0xC0A90000)) {
        //A类私有IP地址：
        //10.0.0.0～10.255.255.255
        //B类私有IP地址：
        //172.16.0.0～172.31.255.255
        //C类私有IP地址：
        //192.168.0.0～192.168.255.255
        //如果是私有地址 说明在nat内部

        /* 优先采用局域网地址，该地址很可能是wifi地址
         * 一般来说,无线路由器分配的地址段是BC类私有ip地址
         * 而A类地址多用于蜂窝移动网络
         */
        return true;
      }
    }
    return false;
  }



#if defined(__APPLE__)

  template<typename FUN>
  void for_each_netAdapter_apple(FUN &&fun) { //type: struct ifaddrs *
    struct ifaddrs *interfaces = nullptr;
    struct ifaddrs *base = nullptr;
    if (getifaddrs(&interfaces) == 0) {
      base = interfaces;
      while (base) {
        if (base->ifa_addr->sa_family == AF_INET) {
          if (fun(base)) {
            break;
          }
        }
        base = base->ifa_next;
      }
      freeifaddrs(interfaces);
    }
  }

#endif //defined(__APPLE__)

#if defined(_WIN32)
  template<typename FUN>
void for_each_netAdapter_win32(FUN && fun) { //type: PIP_ADAPTER_INFO
  unsigned long nSize = sizeof(IP_ADAPTER_INFO);
  PIP_ADAPTER_INFO adapterList = (PIP_ADAPTER_INFO)new char[nSize];
  int nRet = GetAdaptersInfo(adapterList, &nSize);
  if (ERROR_BUFFER_OVERFLOW == nRet) {
      delete[] adapterList;
      adapterList = (PIP_ADAPTER_INFO)new char[nSize];
      nRet = GetAdaptersInfo(adapterList, &nSize);
  }
  auto adapterPtr = adapterList;
  while (adapterPtr && ERROR_SUCCESS == nRet) {
      if (fun(adapterPtr)) {
          break;
      }
      adapterPtr = adapterPtr->Next;
  }
  //释放内存空间
  delete[] adapterList;
}
#endif //defined(_WIN32)

#if !defined(_WIN32) && !defined(__APPLE__)
  template<typename FUN>
void for_each_netAdapter_posix(FUN &&fun){ //type: struct ifreq *
  struct ifconf ifconf;
  char buf[1024 * 10];
  //初始化ifconf
  ifconf.ifc_len = sizeof(buf);
  ifconf.ifc_buf = buf;
  int sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
      LOG_WARN << "Create socket failed: " << GetErrorMsg(true);
      return;
  }
  if (-1 == ioctl(sockfd, SIOCGIFCONF, &ifconf)) {    //获取所有接口信息
      LOG_WARN << "ioctl SIOCGIFCONF failed: " << GetErrorMsg(true);
      close(sockfd);
      return;
  }
  close(sockfd);
  //接下来一个一个的获取IP地址
  struct ifreq * base = (struct ifreq*) buf;
  for (int i = (ifconf.ifc_len / sizeof(struct ifreq)); i > 0; --i,++base) {
      if(fun(base)){
          break;
      }
  }
}
#endif //!defined(_WIN32) && !defined(__APPLE__)

  string GetLocalIP() {
#if defined(__APPLE__)
    string address = "127.0.0.1";
    for_each_netAdapter_apple([&](struct ifaddrs *base) {
      string ip = InetNToa(base->ifa_addr);
      if (strstr(base->ifa_name, "docker")) {
        return false;
      }
      return CheckIP(address, ip);
    });
    return address;
#elif defined(_WIN32)
    string address = "127.0.0.1";
  for_each_netAdapter_win32([&](PIP_ADAPTER_INFO base) {
      IP_ADDR_STRING *ipAddr = &(base->IpAddressList);
      while (ipAddr) {
          string ip = ipAddr->IpAddress.String;
          if (strstr(base->AdapterName, "docker")) {
              return false;
          }
          if(check_ip(address,ip)){
              return true;
          }
          ipAddr = ipAddr->Next;
      }
      return false;
  });
  return address;
#else
  string address = "127.0.0.1";
  for_each_netAdapter_posix([&](struct ifreq *base){
      string ip = InetNToa(&(base->ifr_addr));
      if (strstr(base->ifr_name, "docker")) {
          return false;
      }
      return check_ip(address,ip);
  });
  return address;
#endif
  }


  int BindUdpSock(const uint16_t port, const char *local_ip, bool enable_reuse) {
    int fd = -1;
    int family = SupportIPv6() ? (IsIPv4(local_ip) ? AF_INET : AF_INET6) : AF_INET;
    if ((fd = (int) socket(family, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
      LOG_WARN << "Create socket failed: " << GetErrorMsg(true);
      return -1;
    }
    if (enable_reuse) {
      SetReuseable(fd);
    }
    SetNoSigpipe(fd);
    SetNoBlocked(fd);
    SetSendBuf(fd);
    SetRecvBuf(fd);
    SetCloseWait(fd);
    SetCloExec(fd);

    if (BindSock(fd, local_ip, port, family) == -1) {
      close(fd);
      return -1;
    }
    return fd;
  }

  int DissolveUdpSock(int fd) {
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    if (-1 == getsockname(fd, (struct sockaddr *) &addr, &addr_len)) {
      return -1;
    }
    addr.ss_family = AF_UNSPEC;
    if (-1 == ::connect(fd, (struct sockaddr *) &addr, addr_len) && GetError() != RD_EAFNOSUPPORT) {
      // mac/ios时返回EAFNOSUPPORT错误
      LOG_WARN << "Connect socket AF_UNSPEC failed: " << GetErrorMsg(true);
      return -1;
    }
    return 0;
  }


  string get_ifr_ip(const char *if_name) {
#if defined(__APPLE__)
    string ret;
    for_each_netAdapter_apple([&](struct ifaddrs *base) {
      if (strcmp(base->ifa_name, if_name) == 0) {
        ret = InetNToa(base->ifa_addr);
        return true;
      }
      return false;
    });
    return ret;
#elif defined(_WIN32)
    string ret;
  for_each_netAdapter_win32([&](PIP_ADAPTER_INFO base) {
      IP_ADDR_STRING *ipAddr = &(base->IpAddressList);
      while (ipAddr){
          if (strcmp(if_name,base->AdapterName) == 0){
              //ip匹配到了
              ret.assign(ipAddr->IpAddress.String);
              return true;
          }
          ipAddr = ipAddr->Next;
      }
      return false;
  });
  return ret;
#else
  string ret;
  for_each_netAdapter_posix([&](struct ifreq *base){
      if(strcmp(base->ifr_name,if_name) == 0) {
          ret = InetNToa(&(base->ifr_addr));
          return true;
      }
      return false;
  });
  return ret;
#endif
  }

  string get_ifr_name(const char *local_ip) {
#if defined(__APPLE__)
    string ret = "en0";
    for_each_netAdapter_apple([&](struct ifaddrs *base) {
      string ip = InetNToa(base->ifa_addr);
      if (ip == local_ip) {
        ret = base->ifa_name;
        return true;
      }
      return false;
    });
    return ret;
#elif defined(_WIN32)
    string ret = "en0";
  for_each_netAdapter_win32([&](PIP_ADAPTER_INFO base) {
      IP_ADDR_STRING *ipAddr = &(base->IpAddressList);
      while (ipAddr){
          if (strcmp(local_ip,ipAddr->IpAddress.String) == 0){
              //ip匹配到了
              ret.assign(base->AdapterName);
              return true;
          }
          ipAddr = ipAddr->Next;
      }
      return false;
  });
  return ret;
#else
  string ret = "en0";
  for_each_netAdapter_posix([&](struct ifreq *base){
      string ip = InetNToa(&(base->ifr_addr));
      if(ip == local_ip) {
          ret = base->ifr_name;
          return true;
      }
      return false;
  });
  return ret;
#endif
  }

  string get_ifr_mask(const char *if_name) {
#if defined(__APPLE__)
    string ret;
    for_each_netAdapter_apple([&](struct ifaddrs *base) {
      if (strcmp(if_name, base->ifa_name) == 0) {
        ret = InetNToa(base->ifa_netmask);
        return true;
      }
      return false;
    });
    return ret;
#elif defined(_WIN32)
    string ret;
  for_each_netAdapter_win32([&](PIP_ADAPTER_INFO base) {
      if (strcmp(if_name,base->AdapterName) == 0){
          //找到了该网卡
          IP_ADDR_STRING *ipAddr = &(base->IpAddressList);
          //获取第一个ip的子网掩码
          ret.assign(ipAddr->IpMask.String);
          return true;
      }
      return false;
  });
  return ret;
#else
  int fd;
  struct ifreq ifr_mask;
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
      LOG_WARN << "Create socket failed: " << GetErrorMsg(true);
      return "";
  }
  memSet(&ifr_mask, 0, sizeof(ifr_mask));
  strncpy(ifr_mask.ifr_name, if_name, sizeof(ifr_mask.ifr_name) - 1);
  if ((ioctl(fd, SIOCGIFNETMASK, &ifr_mask)) < 0) {
      LOG_WARN << "ioctl SIOCGIFNETMASK on " << if_name << " failed: " << GetErrorMsg(true);
      close(fd);
      return "";
  }
  close(fd);
  return InetNToa(&(ifr_mask.ifr_netmask));
#endif // defined(_WIN32)
  }

  string get_ifr_brdaddr(const char *if_name) {
#if defined(__APPLE__)
    string ret;
    for_each_netAdapter_apple([&](struct ifaddrs *base) {
      if (strcmp(if_name, base->ifa_name) == 0) {
        ret = InetNToa(base->ifa_broadaddr);
        return true;
      }
      return false;
    });
    return ret;
#elif defined(_WIN32)
    string ret;
  for_each_netAdapter_win32([&](PIP_ADAPTER_INFO base) {
      if (strcmp(if_name, base->AdapterName) == 0) {
          //找到该网卡
          IP_ADDR_STRING *ipAddr = &(base->IpAddressList);
          in_addr broadcast;
          broadcast.S_un.S_addr = (inet_addr(ipAddr->IpAddress.String) & inet_addr(ipAddr->IpMask.String)) | (~inet_addr(ipAddr->IpMask.String));
          ret = InetNToa(broadcast);
          return true;
      }
      return false;
  });
  return ret;
#else
  int fd;
  struct ifreq ifr_mask;
  fd = socket( AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
      LOG_WARN << "Create socket failed: " << GetErrorMsg(true);
      return "";
  }
  memSet(&ifr_mask, 0, sizeof(ifr_mask));
  strncpy(ifr_mask.ifr_name, if_name, sizeof(ifr_mask.ifr_name) - 1);
  if ((ioctl(fd, SIOCGIFBRDADDR, &ifr_mask)) < 0) {
      LOG_WARN << "ioctl SIOCGIFBRDADDR failed: " << GetErrorMsg(true);
      close(fd);
      return "";
  }
  close(fd);
  return InetNToa(&(ifr_mask.ifr_broadaddr));
#endif
  }

#define ip_addr_netcmp(addr1, addr2, mask) (((addr1) & (mask)) == ((addr2) & (mask)))

  bool in_same_lan(const char *myIp, const char *dstIp) {
    string mask = get_ifr_mask(get_ifr_name(myIp).data());
    return ip_addr_netcmp(inet_addr(myIp), inet_addr(dstIp), inet_addr(mask.data()));
  }

  void clearMulticastAllSocketOption(int socket) {
#if defined(IP_MULTICAST_ALL)
    // This option is defined in modern versions of Linux to overcome a bug in the Linux kernel's default behavior.
  // When Set to 0, it ensures that we receive only packets that were sent to the specified IP multicast address,
  // even if some other process on the same system has joined a different multicast group with the same port number.
  int multicastAll = 0;
  (void)setsockopt(socket, IPPROTO_IP, IP_MULTICAST_ALL, (void*)&multicastAll, sizeof multicastAll);
  // Ignore the call's result.  Should it fail, we'll still receive packets (just perhaps more than intended)
#endif
  }

  int SetMultiTTL(int fd, uint8_t ttl) {
    int ret = -1;
#if defined(IP_MULTICAST_TTL)
    ret = setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, (char *) &ttl, sizeof(ttl));
    if (ret == -1) {
      LOG_TRACE << "setsockopt IP_MULTICAST_TTL failed";
    }
#endif
    clearMulticastAllSocketOption(fd);
    return ret;
  }

  int SetMultiIF(int fd, const char *local_ip) {
    int ret = -1;
#if defined(IP_MULTICAST_IF)
    struct in_addr addr;
    addr.s_addr = inet_addr(local_ip);
    ret = setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, (char *) &addr, sizeof(addr));
    if (ret == -1) {
      LOG_TRACE << "setsockopt IP_MULTICAST_IF failed";
    }
#endif
    clearMulticastAllSocketOption(fd);
    return ret;
  }

  int SetMultiLOOP(int fd, bool accept) {
    int ret = -1;
#if defined(IP_MULTICAST_LOOP)
    uint8_t loop = accept;
    ret = setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *) &loop, sizeof(loop));
    if (ret == -1) {
      LOG_TRACE << "setsockopt IP_MULTICAST_LOOP failed";
    }
#endif
    clearMulticastAllSocketOption(fd);
    return ret;
  }

  int joinMultiAddr(int fd, const char *addr, const char *local_ip) {
    int ret = -1;
#if defined(IP_ADD_MEMBERSHIP)
    struct ip_mreq imr;
    imr.imr_multiaddr.s_addr = inet_addr(addr);
    imr.imr_interface.s_addr = inet_addr(local_ip);
    ret = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &imr, sizeof(struct ip_mreq));
    if (ret == -1) {
      LOG_TRACE << "setsockopt IP_ADD_MEMBERSHIP failed: " << GetErrorMsg(true);
    }
#endif
    clearMulticastAllSocketOption(fd);
    return ret;
  }

  int leaveMultiAddr(int fd, const char *addr, const char *local_ip) {
    int ret = -1;
#if defined(IP_DROP_MEMBERSHIP)
    struct ip_mreq imr;
    imr.imr_multiaddr.s_addr = inet_addr(addr);
    imr.imr_interface.s_addr = inet_addr(local_ip);
    ret = setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *) &imr, sizeof(struct ip_mreq));
    if (ret == -1) {
      LOG_TRACE << "setsockopt IP_DROP_MEMBERSHIP failed: " << GetErrorMsg(true);
    }
#endif
    clearMulticastAllSocketOption(fd);
    return ret;
  }

  template<typename A, typename B>
  static inline void write4Byte(A &&a, B &&b) {
    memcpy(&a, &b, sizeof(a));
  }

  int joinMultiAddrFilter(int fd, const char *addr, const char *src_ip, const char *local_ip) {
    int ret = -1;
#if defined(IP_ADD_SOURCE_MEMBERSHIP)
    struct ip_mreq_source imr;

    write4Byte(imr.imr_multiaddr, inet_addr(addr));
    write4Byte(imr.imr_sourceaddr, inet_addr(src_ip));
    write4Byte(imr.imr_interface, inet_addr(local_ip));

    ret = setsockopt(fd, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP, (char *) &imr, sizeof(struct ip_mreq_source));
    if (ret == -1) {
      LOG_TRACE << "setsockopt IP_ADD_SOURCE_MEMBERSHIP failed: " << GetErrorMsg(true);
    }
#endif
    clearMulticastAllSocketOption(fd);
    return ret;
  }

  int leaveMultiAddrFilter(int fd, const char *addr, const char *src_ip, const char *local_ip) {
    int ret = -1;
#if defined(IP_DROP_SOURCE_MEMBERSHIP)
    struct ip_mreq_source imr;

    write4Byte(imr.imr_multiaddr, inet_addr(addr));
    write4Byte(imr.imr_sourceaddr, inet_addr(src_ip));
    write4Byte(imr.imr_interface, inet_addr(local_ip));

    ret = setsockopt(fd, IPPROTO_IP, IP_DROP_SOURCE_MEMBERSHIP, (char *) &imr, sizeof(struct ip_mreq_source));
    if (ret == -1) {
      LOG_TRACE << "setsockopt IP_DROP_SOURCE_MEMBERSHIP failed: " << GetErrorMsg(true);
    }
#endif
    clearMulticastAllSocketOption(fd);
    return ret;
  }

NET_NAMESPACE_END

#endif //!defined(_WIN32) && !defined(__APPLE__)  //posix