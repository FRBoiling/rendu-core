/*
* Created by boil on 2023/11/4.
*/
#if defined(__APPLE__)

/* For POSIX systems we use the standard BSD socket API. */

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

// apple
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/uio.h>
#include <sys/filio.h>
#include <sys/event.h>

#include "utils/socket_helper.h"
#include "logger/log.h"
#include "utils/rd_errno.h"
#include <unistd.h>

RD_NAMESPACE_BEGIN

#define SOCKET int32

  namespace SocketHelper {
    int Pipe(int fd[2]){
      return pipe(fd);
    }

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

    string GetLocalIP() {
      string address = "127.0.0.1";
      for_each_netAdapter_apple([&](struct ifaddrs *base) {
        string ip = InetNToa(base->ifa_addr);
        if (strstr(base->ifa_name, "docker")) {
          return false;
        }
        return CheckIP(address, ip);
      });
      return address;
    }

    string get_ifr_ip(const char *if_name) {
      string ret;
      for_each_netAdapter_apple([&](struct ifaddrs *base) {
        if (strcmp(base->ifa_name, if_name) == 0) {
          ret = InetNToa(base->ifa_addr);
          return true;
        }
        return false;
      });
      return ret;
    }

    string get_ifr_name(const char *local_ip) {
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
    }

    string get_ifr_mask(const char *if_name) {
      string ret;
      for_each_netAdapter_apple([&](struct ifaddrs *base) {
        if (strcmp(if_name, base->ifa_name) == 0) {
          ret = InetNToa(base->ifa_netmask);
          return true;
        }
        return false;
      });
      return ret;
    }

    string get_ifr_brdaddr(const char *if_name) {
      string ret;
      for_each_netAdapter_apple([&](struct ifaddrs *base) {
        if (strcmp(if_name, base->ifa_name) == 0) {
          ret = InetNToa(base->ifa_broadaddr);
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