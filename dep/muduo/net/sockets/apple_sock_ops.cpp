/*
* Created by boil on 2023/11/4.
*/
#if defined(__APPLE__)

#include "sock_ops.h"
#include "apple_sock.h"
#include "log/log.h"
#include "errno/errno.h"

RD_NAMESPACE_BEGIN

  namespace SockOps {

    int SetNoBlocked(int fd, bool noblock) {
      int ul = noblock;
      int ret = ioctl(fd, FIONBIO, &ul); //设置为非阻塞模式
      if (ret == -1) {
        LOG_TRACE << "ioctl FIONBIO failed";
      }
      return ret;
    }

    int SetCloExec(int fd, bool on) {
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
    }

    int SetKeepAlive(int fd, bool on, int interval, int idle, int times) {
      // Enable/disable the keep-alive option
      int opt = on ? 1 : 0;
      int ret = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &opt, static_cast<socklen_t>(sizeof(opt)));
      if (ret == -1) {
        LOG_TRACE << "setsockopt SO_KEEPALIVE failed";
      }
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
      return ret;
    }

#if defined(__APPLE__)

    template<typename FUN>
    void for_each_netAdapter_apple(FUN &&fun) { //type: struct ifaddrs *
      struct ifaddrs *interfaces = nullptr;
      struct ifaddrs *adapter = nullptr;
      if (getifaddrs(&interfaces) == 0) {
        adapter = interfaces;
        while (adapter) {
          if (adapter->ifa_addr->sa_family == AF_INET) {
            if (fun(adapter)) {
              break;
            }
          }
          adapter = adapter->ifa_next;
        }
        freeifaddrs(interfaces);
      }
    }

#endif //defined(__APPLE__)

    string GetLocalIP() {
      string address = "127.0.0.1";
      for_each_netAdapter_apple([&](struct ifaddrs *adapter) {
        string ip = InetNToa(adapter->ifa_addr);
        if (strstr(adapter->ifa_name, "docker")) {
          return false;
        }
        return CheckIP(address, ip);
      });
      return address;
    }

    string get_ifr_ip(const char *if_name) {
      string ret;
      for_each_netAdapter_apple([&](struct ifaddrs *adapter) {
        if (strcmp(adapter->ifa_name, if_name) == 0) {
          ret = InetNToa(adapter->ifa_addr);
          return true;
        }
        return false;
      });
      return ret;
    }

    string get_ifr_name(const char *local_ip) {
      string ret = "en0";
      for_each_netAdapter_apple([&](struct ifaddrs *adapter) {
        string ip = InetNToa(adapter->ifa_addr);
        if (ip == local_ip) {
          ret = adapter->ifa_name;
          return true;
        }
        return false;
      });
      return ret;
    }

    string get_ifr_mask(const char *if_name) {
      string ret;
      for_each_netAdapter_apple([&](struct ifaddrs *adapter) {
        if (strcmp(if_name, adapter->ifa_name) == 0) {
          ret = InetNToa(adapter->ifa_netmask);
          return true;
        }
        return false;
      });
      return ret;
    }

    string get_ifr_brdaddr(const char *if_name) {
      string ret;
      for_each_netAdapter_apple([&](struct ifaddrs *adapter) {
        if (strcmp(if_name, adapter->ifa_name) == 0) {
          ret = InetNToa(adapter->ifa_broadaddr);
          return true;
        }
        return false;
      });
      return ret;
    }

  }

  ssize_t Readv(int sockfd, const struct iovec *iov, int iovcnt) {
    return readv(sockfd, iov, iovcnt);
  }

RD_NAMESPACE_END

#endif //defined(__APPLE__)