/*
* Created by boil on 2023/11/4.
*/

#include "sock_ops.h"
#include "logger/log.h"
#include "dns_cache.h"
#include "errno/errno.h"
#include <unistd.h>

COMMON_NAMESPACE_BEGIN
  namespace SockOps {

    int CreateNonblockingOrDie(sa_family_t family) {
      int sockfd = socket(family, SOCK_STREAM, IPPROTO_TCP);
      if (sockfd < 0) {
        LOG_SYSFATAL << "sockets::createNonblockingOrDie";
      }
      SetNoBlocked(sockfd);
      SetCloExec(sockfd);
      return sockfd;
    }

    void Close(const int32 sfd) {
      close(sfd);
    }

    void ShutdownWrite(SOCKET sockfd) {
      if (shutdown(sockfd, SHUT_WR) < 0) {
        LOG_SYSERR << "SocketOps::shutdownWrite";
      }
    }

    int BindSock6(int fd, uint16_t port, const char *ifr_ip) {
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
      if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        LOG_WARN << "Bind socket failed: " << GetErrorMsg(true);
        return -1;
      }
      return 0;
    }

    int BindSock4(int fd, uint16_t port, const char *ifr_ip) {
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
      if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        LOG_WARN << "Bind socket failed: " << GetErrorMsg(true);
        return -1;
      }
      return 0;
    }

    int BindSock(int fd, uint16_t port, const char *ifr_ip, int family) {
      switch (family) {
        case AF_INET:
          return BindSock4(fd, port, ifr_ip);
        case AF_INET6:
          return BindSock6(fd, port, ifr_ip);
        default:
          return -1;
      }
    }

    SOCKET Bind(uint16_t port, const char *ifr_ip, int family) {
      SOCKET sfd = socket(family, SOCK_STREAM, IPPROTO_TCP);
      if (sfd == -1) {
        return sfd;
      }
      switch (family) {
        case AF_INET:
          return BindSock4(sfd, port, ifr_ip);
        case AF_INET6:
          return BindSock6(sfd, port, ifr_ip);
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

      if (BindSock(fd, port, local_ip, family) == -1) {
        close(fd);
        return -1;
      }

      //开始监听
      if (listen(fd, back_log) == -1) {
        LOG_WARN << "Listen socket failed: " << GetErrorMsg(true);
        close(fd);
        return -1;
      }

      return fd;
    }

    SOCKET Accept(const SOCKET listen_sfd) {
      SOCKET sockfd = -1;
      sockfd = accept(listen_sfd, nullptr, nullptr);
      return sockfd;
    }

    SOCKET Accept(const SOCKET listen_sfd, struct sockaddr_storage &storage, socklen_t &addr_len) {
      SOCKET sockfd = -1;
      sockfd = accept(listen_sfd, (struct sockaddr *) &storage, &addr_len);
      return sockfd;
    }

    SOCKET Connect(const SOCKET sockfd, struct sockaddr_storage &storage) {
      if (connect(sockfd, (sockaddr *) &storage, GetSockLen((sockaddr *) &storage)) != 0) {
        return -1;
      }
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

      if (BindSock(sockfd, local_port, local_ip, addr.ss_family) == -1) {
        close(sockfd);
        return -1;
      }

      if (connect(sockfd, (sockaddr *) &addr, GetSockLen((sockaddr *) &addr)) == 0) {
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

    bool IsSelfConnect(int sockfd) {
      struct sockaddr_storage localaddr;
      GetSockLocalAddr(sockfd, localaddr);
      struct sockaddr_storage peeraddr;
      GetSockPeerAddr(sockfd, peeraddr);
      if (localaddr.ss_family == AF_INET) {
        auto &laddr4 = reinterpret_cast<struct sockaddr_in & >(localaddr);
        auto &raddr4 = reinterpret_cast<struct sockaddr_in & >(peeraddr);
        return laddr4.sin_port == raddr4.sin_port
               && laddr4.sin_addr.s_addr == raddr4.sin_addr.s_addr;
      } else if (localaddr.ss_family == AF_INET6) {
        auto &laddr6 = reinterpret_cast<struct sockaddr_in6 &>(localaddr);
        auto &raddr6 = reinterpret_cast<struct sockaddr_in6 &>(peeraddr);
        return laddr6.sin6_port == raddr6.sin6_port
               && (memcmp(&laddr6.sin6_addr, &raddr6.sin6_addr, sizeof laddr6.sin6_addr) == 0);
      } else {
        return false;
      }
    }

    ssize_t Read(int sockfd, void *buf, size_t count) {
      return read(sockfd, buf, count);
    }

    ssize_t Readv(int sockfd, const struct iovec *iov, int iovcnt) {
      return readv(sockfd, iov, iovcnt);
    }

    ssize_t Write(int sockfd, const void *buf, size_t count) {
      return write(sockfd, buf, count);
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

    int SetIpv6Only(int fd, bool flag) {
      int opt = flag;
      int ret = setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, (char *) &opt, sizeof opt);
      if (ret == -1) {
        LOG_TRACE << "setsockopt IPV6_V6ONLY failed";
      }
      return ret;
    }

    socklen_t GetSockLen(int family) {
      switch (family) {
        case AF_INET :
          return sizeof(sockaddr_in);
        case AF_INET6 :
          return sizeof(sockaddr_in6);
        default:
          assert(0);
          return 0;
      }
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
      int opt = on ? 1 : 0;
      int ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *) &opt, static_cast<socklen_t>(sizeof(opt)));
      if (ret == -1) {
        LOG_TRACE << "setsockopt TCP_NODELAY failed";
      }
      return ret;
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

    int SetCloseWait(int fd, int second) {
      linger m_sLinger;
      //在调用closesocket()时还有数据未发送完，允许等待
      // 若m_sLinger.l_onoff=0;则调用closesocket()后强制关闭
      m_sLinger.l_onoff = (second > 0);
      m_sLinger.l_linger = second; //设置等待时间为x秒
      int ret = setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *) &m_sLinger, sizeof(linger));
      if (ret == -1) {
        LOG_TRACE << "setsockopt SO_LINGER failed";
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

      if (BindSock(fd, port, local_ip, family) == -1) {
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

  }

RD_NAMESPACE_END