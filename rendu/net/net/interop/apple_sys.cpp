/*
* Created by boil on 2024/2/17.
*/

#if defined(__APPLE__)

#include "apple_sys.h"

#include <unistd.h>

#include "interop/error_code.h"
#include "interop/sys.h"

NET_NAMESPACE_BEGIN

namespace interop {

  int Sys::Close(int fd) {
    return ::close(fd);
  }

  string Sys::GetHostName() {
    constexpr int MaxHostNameLength = static_cast<int>(std::numeric_limits<byte>::max());
    byte hostname[MaxHostNameLength];
    if (GetHostName(hostname, MaxHostNameLength) != 0) {
      throw std::runtime_error("Error while getting the host name!");
    }
    return reinterpret_cast<char *>(hostname);
  }

  int Sys::GetHostName(byte *name, int nameLength) {
    return ::gethostname((char *) name, nameLength);
  }


  static Error GetIpAddress(const byte *sock_addr, string &ip_address) {
    sockaddr &tmp_addr = (struct sockaddr &) *sock_addr;
    switch (tmp_addr.sa_family) {
      case AF_UNIX: {
        struct sockaddr_un *sockaddr_un = reinterpret_cast<struct sockaddr_un *>(&tmp_addr);
        ip_address = sockaddr_un->sun_path;
        break;
      }
      case AF_INET: {
        char addr_buff[INET_ADDRSTRLEN];// Use INET6_ADDRSTRLEN to accommodate IPv6 addresses
        struct sockaddr_in *sockaddr_in = reinterpret_cast<struct sockaddr_in *>(&tmp_addr);
        if (::inet_ntop(AF_INET, &(sockaddr_in->sin_addr), addr_buff, INET_ADDRSTRLEN) != nullptr) {
          ip_address = addr_buff;
        }
        break;
      }
      case AF_INET6: {
        char addr_buff[INET6_ADDRSTRLEN];
        struct sockaddr_in6 *sockaddr_in6 = reinterpret_cast<struct sockaddr_in6 *>(&tmp_addr);
        if (::inet_ntop(AF_INET6, &(sockaddr_in6->sin6_addr), addr_buff, INET6_ADDRSTRLEN) != nullptr) {
          ip_address = addr_buff;
        }
        break;
      }
      default:
        RD_CRITICAL("Unsupported address family: {} ", tmp_addr.sa_family);
        return Error::RD_FAIL;
    }
    return Error::RD_SUCCESS;
  }

  static Error GetPort(const byte *sock_addr, ushort &port) {
    sockaddr &tmp_addr = (struct sockaddr &) *sock_addr;
    switch (tmp_addr.sa_family) {
      case AF_INET: {
        struct sockaddr_in &sockaddr_v4 = (struct sockaddr_in &) sock_addr;
        port = ntohs(sockaddr_v4.sin_port);
        break;
      }
      case AF_INET6: {
        struct sockaddr_in6 &sockaddr_v6 = (struct sockaddr_in6 &) sock_addr;
        port = ntohs(sockaddr_v6.sin6_port);
        break;
      }
      default:
        RD_CRITICAL("Unsupported address family: {} ", tmp_addr.sa_family);
        return Error::RD_FAIL;
    }
    return Error::RD_SUCCESS;
  }

  Error Sys::GetSockName(SafeHandle::Ptr socket, byte *sock_addr, int &addr_len) {
    sockaddr &tmp_addr = (struct sockaddr &) *sock_addr;
    socklen_t tmp_addr_len = sizeof(sockaddr);
    int raw_error = ::getsockname(socket->RawHandle(), &tmp_addr, &tmp_addr_len);
    if (raw_error == -1) {
      return GetLastError();
    }
    addr_len = tmp_addr_len;
    return Error::RD_SUCCESS;
  }

  Error Sys::GetPeerName(SafeHandle::Ptr socket, byte *sock_addr, int &addr_len) {
    sockaddr &tmp_addr = (struct sockaddr &) *sock_addr;
    socklen_t tmp_addr_len = sizeof(sockaddr);
    int raw_error = ::getpeername(socket->RawHandle(), &tmp_addr, &tmp_addr_len);
    if (raw_error == -1) {
      return GetLastError();
    }

    addr_len = tmp_addr_len;
    return Error::RD_SUCCESS;
  }

  Error Sys::Socket(int addressFamily, int socketType, int protocolType, int &socket_fd) {
    //  int m_acceptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int raw_fd = ::socket(addressFamily, socketType, protocolType);
    if (raw_fd == -1) {
      return Error::RD_FAIL;
    }
    socket_fd = raw_fd;
    return Error::RD_SUCCESS;
  }

  Error Sys::GetLastError() {
    return ConvertToError(errno);
  }


  static Error GetSocketAddressSizes(int *ipv4SocketAddressSize, int *ipv6SocketAddressSize, int *udsSocketAddressSize, int *maxSocketAddressSize) {
    if (ipv4SocketAddressSize != NULL) {
      //获取IPv4的socket地址大小
      *ipv4SocketAddressSize = sizeof(sockaddr_in);
    }

    if (ipv6SocketAddressSize != NULL) {
      //获取IPv6的socket地址大小
      *ipv6SocketAddressSize = sizeof(sockaddr_in6);
    }

    if (udsSocketAddressSize != NULL) {
      //获取Unix Domain Sockets的socket地址大小
      *udsSocketAddressSize = sizeof(sockaddr_un);
    }

    if (maxSocketAddressSize != NULL) {
      //获取最大的socket地址大小
      //我们使用std::max函数，这将需要include<algorithm>
      *maxSocketAddressSize = std::max({*ipv4SocketAddressSize, *ipv6SocketAddressSize, *udsSocketAddressSize});
    }
    return Error::RD_SUCCESS;
  }

  Error Sys::GetAddressFamily(byte *socketAddress, int socketAddressLen, int &addressFamily) {
    struct sockaddr_storage *sockaddr_storage = (struct sockaddr_storage *) socketAddress;
    addressFamily = sockaddr_storage->ss_family;
    return Error::RD_SUCCESS;
  }
  Error Sys::SetAddressFamily(byte *socketAddress, int socketAddressLen, int addressFamily) {
    struct sockaddr_storage *sockaddr_storage = (struct sockaddr_storage *) socketAddress;
    sockaddr_storage->ss_family = addressFamily;
    //    sockaddr_storage->ss_len = socketAddressLen;
    return Error::RD_SUCCESS;
  }

  Error Sys::SetPort(byte *socketAddress, int socketAddressLen, ushort port) {
    if (socketAddressLen >= sizeof(sockaddr_in)) {
      auto *socStorage = reinterpret_cast<struct sockaddr_storage *>(socketAddress);
      if (socStorage->ss_family == AF_INET) {// IPv4地址
        auto *socAddress4 = reinterpret_cast<struct sockaddr_in *>(socketAddress);
        socAddress4->sin_port = htons(port);         //使用htons函数转换端口号的字节序
      } else if (socStorage->ss_family == AF_INET6) {//IPv6地址
        auto *socAddress6 = reinterpret_cast<struct sockaddr_in6 *>(socketAddress);
        socAddress6->sin6_port = htons(port);
      } else {
        RD_ERROR("not supported address family")
        return Error::RD_FAIL;//不支持的协议
      }
    } else {
      RD_ERROR("invalid socket address length")
      return Error::RD_FAIL;//无效的长度
    }
    return Error::RD_SUCCESS;//成功修改端口
  }

  Error Sys::GetPort(byte *socketAddress, int socketAddressLen, ushort *port) {
    // 检查 socketAddressLen 是否至少为 sockaddr_in 的大小
    if (socketAddressLen >= sizeof(sockaddr_in)) {
      auto *socStorage = reinterpret_cast<const struct sockaddr_storage *>(socketAddress);
      if (socStorage->ss_family == AF_INET) {// IPv4地址
        auto *socAddress4 = reinterpret_cast<const struct sockaddr_in *>(socketAddress);
        *port = ntohs(socAddress4->sin_port);        //使用ntohs函数转换回网络字节序
      } else if (socStorage->ss_family == AF_INET6) {// IPv6地址
        auto *socAddress6 = reinterpret_cast<const struct sockaddr_in6 *>(socketAddress);
        *port = ntohs(socAddress6->sin6_port);
      } else {
        RD_ERROR("not supported address family")
        return Error::RD_FAIL;//不支持的协议
      }
    } else {
      RD_ERROR("invalid socket address length")
      return Error::RD_FAIL;//无效的长度
    }
    return Error::RD_SUCCESS;//成功获取
  }

  Error Sys::GetIPv4Address(byte *socketAddress, int socketAddressLen, uint *address) {
    // 检查 socketAddressLen 是否至少为 sockaddr_in 的大小
    if (socketAddressLen >= sizeof(sockaddr_in)) {
      auto *socStorage = reinterpret_cast<const struct sockaddr_storage *>(socketAddress);
      if (socStorage->ss_family == AF_INET) {// IPv4地址
        auto *socAddress4 = reinterpret_cast<const struct sockaddr_in *>(socketAddress);
        *address = ntohl(socAddress4->sin_addr.s_addr);// 使用ntohl函数转换网络字节序到主机字节序
      } else {
        RD_ERROR("not supported address family")
        return Error::RD_FAIL;//不支持的协议
      }
    } else {
      RD_ERROR("invalid socket address length")
      return Error::RD_FAIL;//无效的长度
    }
    return Error::RD_SUCCESS;//成功获取IPv4地址
  }

  Error Sys::SetIPv4Address(byte *socketAddress, int socketAddressLen, uint address) {
    // 检查 socketAddressLen 是否至少为 sockaddr_in 的大小
    if (socketAddressLen == sizeof(sockaddr_in)) {
      auto *sockAddress4 = reinterpret_cast<struct sockaddr_in *>(socketAddress);
      if (sockAddress4->sin_family == AF_INET) {       // IPv4 address
        sockAddress4->sin_addr.s_addr = htonl(address);// 使用htonl函数将主机字节序的地址转换成网络字节序
      } else {
        RD_ERROR("not supported address family")
        return Error::RD_FAIL;// 不支持的协议
      }
    } else {
      RD_ERROR("invalid socket address length")
      return Error::RD_FAIL;// 无效的长度
    }
    return Error::RD_SUCCESS;// 成功设置IPv4地址
  }

  Error Sys::SetIPv6Address(byte *socketAddress, int socketAddressLen, byte *address, int addressLen, uint scopeId) {
    // 检查 socketAddressLen 是否至少为 sockaddr_in6 的大小
    if (socketAddressLen == sizeof(sockaddr_in6)) {
      auto *socAddress6 = reinterpret_cast<struct sockaddr_in6 *>(socketAddress);
      if (socAddress6->sin6_family == AF_INET6) {// IPv6 address
        if (addressLen == 16) {
          memcpy(&socAddress6->sin6_addr, address, 16);// Copy IPv6 address
          socAddress6->sin6_scope_id = scopeId;        // Set scope ID
        } else {
          RD_ERROR("invalid address length for IPv6")
          return Error::RD_FAIL;// Invalid address length for IPv6
        }
      } else {
        RD_ERROR("not supported address family")
        return Error::RD_FAIL;// Unsupported protocol
      }
    } else {
      RD_ERROR("invalid socket address length")
      return Error::RD_FAIL;// Invalid length
    }
    return Error::RD_SUCCESS;// Successfully set IPv6 address
  }

  Error Sys::GetIPv6Address(byte *socketAddress, int socketAddressLen, byte *address, int addressLen, uint *scopeId) {
    // 检查 socketAddressLen 是否至少为 sockaddr_in6 的大小
    if (socketAddressLen >= sizeof(sockaddr_in6)) {
      auto *socStorage = reinterpret_cast<struct sockaddr_storage *>(socketAddress);
      if (socStorage->ss_family == AF_INET6) {// IPv6 address
        auto *socAddress6 = reinterpret_cast<struct sockaddr_in6 *>(socketAddress);
        if (addressLen == 16) {
          memcpy(address, &socAddress6->sin6_addr, 16);// Copy IPv6 address to output
          *scopeId = socAddress6->sin6_scope_id;       // Copy scope ID to output
        } else {
          RD_ERROR("invalid address length for IPv6")
          return Error::RD_FAIL;// Invalid address length for IPv6
        }
      } else {
        RD_ERROR("not supported address family")
        return Error::RD_FAIL;// Unsupported protocol
      }
    } else {
      RD_ERROR("invalid socket address length")
      return Error::RD_FAIL;// Invalid length
    }
    return Error::RD_SUCCESS;// Successfully get IPv6 address and scope ID
  }


  Error Sys::GetSocketErrorOption(SafeHandle::Ptr socket, Error &error) {
    int socket_native_err = -1;
    socklen_t len = sizeof(socket_native_err);
    Error socketError = Error::RD_SUCCESS;
    // 获取SO_ERROR选项的值
    int retval = getsockopt(socket->RawHandle(), SOL_SOCKET, SO_ERROR, &socket_native_err, &len);
    if (retval != 0) {
      // 在getsockopt()函数调用过程中产生错误
      error = GetLastError();
      RD_ERROR("getsockopt() error: {}", socketError);
      return socketError;
    }

    if (socket_native_err != 0) {
      RD_TRACE("socket_native_err: {}", socket_native_err);
      error = ConvertToError(socket_native_err);
      return socketError;
    }
    RD_TRACE("getsockopt: {}", socket_native_err);
    return socketError;
  }

  Error Sys::SetSockOpt(int socket,
                        SocketOptionLevel optionLevel,// 通信层级
                        SocketOptionName optionName,  // 选项名
                        byte *optionValue,            // 指向要设定选项值的缓冲区的指针
                        int optionLen)                // 缓冲区长度
  {
    int res = ::setsockopt(socket, (int) optionLevel, (int) optionName, (char *) optionValue, optionLen);
    if (res == -1) {
      RD_ERROR("setsockopt() error: {}", res);
      return Error::RD_FAIL;
    }
    return Error::RD_SUCCESS;
  }


  Error Sys::SetSockOpt(SafeHandle::Ptr socket, SocketOptionLevel optionLevel, SocketOptionName optionName, byte *optionValue, int optionLen) {
    return SetSockOpt(socket->RawHandle(), optionLevel, optionName, optionValue, optionLen);
  }

  Error Sys::GetSockOpt(int socket, SocketOptionLevel optionLevel, SocketOptionName optionName, byte *optionValue, int *optionLen) {
    int res = ::getsockopt(socket, (int) optionLevel, (int) optionName, (char *) optionValue, (socklen_t *) optionLen);
    if (res == -1) {
      RD_ERROR("getsockopt() error: {}", res);
      return Error::RD_FAIL;
    }
    return Error::RD_SUCCESS;
  }


  Error Sys::GetSockOpt(SafeHandle::Ptr socket, SocketOptionLevel optionLevel, SocketOptionName optionName, byte *optionValue, int *optionLen) {
    return GetSockOpt(socket->RawHandle(), optionLevel, optionName, optionValue, optionLen);
  }

  Error Sys::Bind(SafeHandle::Ptr socket, ProtocolType socketProtocolType, std::span<byte> socket_address) {
    return Bind(socket, socketProtocolType, socket_address, socket_address.size());
  }

  Error Sys::Bind(SafeHandle::Ptr socket, ProtocolType socketProtocolType, std::span<byte> socketAddress, int socketAddressLen) {
    int res = 0;
    switch (socketProtocolType) {
      case ProtocolType::Tcp:
        if (socketAddressLen == sizeof(struct sockaddr_in)) {
          struct sockaddr_in *sockaddr_in4 = (struct sockaddr_in *) socketAddress.data();
          res = ::bind(socket->RawHandle(), (struct sockaddr *) sockaddr_in4, socketAddressLen);
          break;
        }
        if (socketAddressLen == sizeof(struct sockaddr_in6)) {
          struct sockaddr_in6 *sockaddr_in6 = (struct sockaddr_in6 *) socketAddress.data();
          res = ::bind(socket->RawHandle(), (struct sockaddr *) sockaddr_in6, socketAddressLen);
          break;
        }
        RD_ERROR("bind() Tcp error: {}", GetLastError());
        break;
      case ProtocolType::IPv4:
        if (socketAddressLen == sizeof(struct sockaddr_in)) {
          struct sockaddr_in *sockaddr_in4 = (struct sockaddr_in *) socketAddress.data();
          res = ::bind(socket->RawHandle(), (struct sockaddr *) sockaddr_in4, socketAddressLen);
        }
        break;
      case ProtocolType::IPv6:
        if (socketAddressLen >= sizeof(struct sockaddr_in6)) {
          struct sockaddr_in6 *sockaddr_in6 = (struct sockaddr_in6 *) socketAddress.data();
          res = ::bind(socket->RawHandle(), (struct sockaddr *) sockaddr_in6, socketAddressLen);
        }
        break;
      default:
        RD_ERROR("bind() error,  SocketProtocolType {}", (int) socketProtocolType);
        return Error::RD_FAIL;
    }
    if (res == -1) {
      perror("bind");
      RD_ERROR("bind() error: {}", res);
      return Error::RD_FAIL;
    }
    return Error::RD_SUCCESS;
  }

  Error Sys::Listen(SafeHandle::Ptr socket, int backlog) {
    int res = ::listen(socket->RawHandle(), backlog);
    if (res == -1) {
      RD_ERROR("listen() error: {}", res);
      return Error::RD_FAIL;
    }
    return Error::RD_SUCCESS;
  }

  Error Sys::Accept(SafeHandle::Ptr socket, byte *socketAddress, int *socketAddressLen, int *acceptedFd) {
    socklen_t addrLen = sizeof(struct sockaddr);
    // Create a new socket for the connection and allocates a new file descriptor for it
    int newSocket = ::accept(socket->RawHandle(), (struct sockaddr *) socketAddress, &addrLen);
    if (newSocket < 0) {
      RD_ERROR("accept() error");
      return Error::RD_FAIL;
    }

    *acceptedFd = newSocket;
    *socketAddressLen = addrLen;
    return Error::RD_SUCCESS;
  }


  Error Sys::Connect(SafeHandle::Ptr socket, byte *socketAddress, int socketAddressLen) {
    int res = ::connect(socket->RawHandle(), (struct sockaddr *) socketAddress, socketAddressLen);
    if (res == -1) {
      RD_ERROR("connect() error: {}", res);
      return Error::RD_FAIL;
    }
    return Error::RD_SUCCESS;
  }

  Error Sys::GetLingerOption(SafeHandle::Ptr socket, LingerOption *&option) {
    socklen_t optlen = sizeof(struct linger);
    struct linger lin_opt;
    Error err = Error::RD_SUCCESS;
    int retval = ::getsockopt(socket->RawHandle(), SOL_SOCKET, SO_LINGER, &lin_opt, &optlen);
    if (retval != 0) {
      // 在getsockopt()函数调用过程中产生错误
      err = GetLastError();
      RD_ERROR("getsockopt() error: {}", err);
      return GetLastError();
      ;
    }
    //Copying over linger values to the option param
    if (lin_opt.l_onoff) {
      option->m_on_off = true;
    } else {
      option->m_on_off = false;
    }
    option->m_seconds = lin_opt.l_linger;
    return err;
  }

  Error Sys::GetIPv4MulticastOption(SafeHandle::Ptr socket, MulticastOption multicastOption, IPv4MulticastOption *&option) {
    //FIXME:
    return Error ::RD_SUCCESS;
  }

  Error Sys::GetIPv6MulticastOption(SafeHandle::Ptr socket, MulticastOption multicastOption, IPv6MulticastOption *&option) {
    //FIXME:
    return Error ::RD_SUCCESS;
  }

  bool Sys::IsIPv6Disabled() {
    int sock = socket(AF_INET6, SOCK_STREAM, 0);
    if (sock < 0) {
      // socket creation failed, IPv6 might be disabled
      return true;
    }

    // Clean up socket
    close(sock);
    return false;
  }

  Error Sys::Poll(PollEvent *pollEvents, uint eventCount, int timeout, uint *triggered) {
    struct kevent changeList[eventCount];
    struct kevent eventList[eventCount];

    for (uint i = 0; i < eventCount; i++) {
      struct kevent ke = {pollEvents[i].FileDescriptor, pollEvents[i].Events, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, nullptr};
      changeList[i] = ke;
    }

    int kq = kqueue();
    if (kq == -1) {
      return Error::RD_FAIL;
    }

    timespec ts;
    ts.tv_sec = timeout / 1000;
    ts.tv_nsec = (timeout % 1000) * 1000000;

    int ret = kevent(kq, changeList, eventCount, eventList, eventCount, &ts);

    if (ret == -1) {
      return Error::RD_FAIL;
    } else {
      for (uint i = 0; i < eventCount; i++) {
        pollEvents[i].TriggeredEvents = eventList[i].fflags;
      }
      *triggered = ret;
      return Error::RD_SUCCESS;
    }
  }


  Error Sys::Poll(SafeHandle::Ptr socket, PollEvents events, int timeout, PollEvents &triggered) {
    PollEvent pollEvent{socket->RawHandle(), events};

    uint triggeredEvents;
    Error err = Poll(&pollEvent, 1, timeout, &triggeredEvents);
    if (err != Error::RD_SUCCESS) {
      return err;
    }
    triggered = (PollEvents) pollEvent.TriggeredEvents;
    return Error::RD_SUCCESS;
  }


  int Sys::Fcntl::SetIsNonBlocking(SafeHandle::Ptr fd, int isNonBlocking) {
    //TODO：BOIL
    int flags = fcntl(fd->RawHandle(), F_GETFL, 0);
    if (flags == -1) {
      return -1;
    }
    return 0;
  }


}// namespace interop


NET_NAMESPACE_END

#endif