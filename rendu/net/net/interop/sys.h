/*
* Created by boil on 2024/2/17.
*/

#ifndef RENDU_NET_NET_INTEROP_SYS_H_
#define RENDU_NET_NET_INTEROP_SYS_H_

#include "net_define.h"

#include "error_code.h"
#include "safe_handle.h"

#include "poller/poll_event.h"
#include "poller/poll_events.h"
#include "sockets/protocol_type.h"
#include "sockets/socket_error.h"
#include "sockets/socket_option_level.h"
#include "sockets/socket_option_name.h"
#include "sockets/socket_shutdown.h"

NET_NAMESPACE_BEGIN

namespace interop {

  class Sys {
  public:
    struct IPAddress {
      byte Address[16];

    private:
      uint _isIPv6;
      uint ScopeId;

    public:
      bool IsIPv6() { return _isIPv6 > 0U; };
    };

    struct LingerOption {
      LingerOption(bool is_on, int seconds = 0) : m_on_off(is_on ? 1 : 0), m_seconds(seconds) {}
      int m_on_off;
      int m_seconds;
    };

    enum class MulticastOption {
      MULTICAST_ADD,
      MULTICAST_DROP,
      MULTICAST_IF,
    };

    struct IPv4MulticastOption {
    public:
      uint MulticastAddress;
      uint LocalAddress;
      int InterfaceIndex;

    private:
      int _padding;
    };

    struct IPv6MulticastOption {
    public:
      interop::Sys::IPAddress Address;
      int InterfaceIndex;

    private:
      int _padding;
    };

    enum FileStatusFlags {
      None = 0,
      HasBirthTime = 1,
    };

    struct FileStatus {
      FileStatusFlags Flags;
      int Mode;
      uint Uid;
      uint Gid;
      long Size;
      long ATime;
      long ATimeNsec;
      long MTime;
      long MTimeNsec;
      long CTime;
      long CTimeNsec;
      long BirthTime;
      long BirthTimeNsec;
      long Dev;
      long RDev;
      long Ino;
      uint UserFlags;
    };


  public:
    static int GetMaximumAddressSize();
    static int Close(int fd);
    static Error GetLastError();

    static string GetHostName();
    static int GetHostName(byte *name, int nameLength);

    static Error GetSockName(SafeHandle::Ptr socket, byte *addr, int &addr_len);
    static Error GetPeerName(SafeHandle::Ptr socket, byte *addr, int &addr_len);

    static Error Socket(int addressFamily, int socketType, int protocolType, int &socket_fd);

    static Error GetSocketErrorOption(SafeHandle::Ptr socket, Error &error);

    static Error SetSockOpt(int socket, SocketOptionLevel optionLevel, SocketOptionName optionName, byte *optionValue, int optionLen);
    static Error SetSockOpt(SafeHandle::Ptr socket, SocketOptionLevel optionLevel, SocketOptionName optionName, byte *optionValue, int optionLen);

    static Error GetSockOpt(int socket, SocketOptionLevel optionLevel, SocketOptionName optionName, byte *optionValue, int *optionLen);
    static Error GetSockOpt(SafeHandle::Ptr socket, SocketOptionLevel optionLevel, SocketOptionName optionName, byte *optionValue, int *optionLen);

    static Error GetAddressFamily(byte *socketAddress, int socketAddressLen, int &addressFamily);
    static Error SetAddressFamily(byte *socketAddress, int socketAddressLen, int addressFamily);

    static Error SetPort(byte *socketAddress, int socketAddressLen, ushort port);
    static Error GetPort(byte *socketAddress, int socketAddressLen, ushort *port);

    static Error GetIPv4Address(byte *socketAddress, int socketAddressLen, uint *address);
    static Error SetIPv4Address(byte *socketAddress, int socketAddressLen, uint address);

    static Error SetIPv6Address(byte *socketAddress, int socketAddressLen, byte *address, int addressLen, uint scopeId);
    static Error GetIPv6Address(byte *socketAddress, int socketAddressLen, byte *address, int addressLen, uint *scopeId);

    static Error Bind(SafeHandle::Ptr socket, ProtocolType socketProtocolType, std::span<byte> socket_address);
    static Error Bind(SafeHandle::Ptr socket, ProtocolType socketProtocolType, std::span<byte> socketAddress, int socketAddressLen);

    static Error Listen(SafeHandle::Ptr socket, int backlog);

    static Error Connect(SafeHandle::Ptr socket, byte *socketAddress, int socketAddressLen);

    static Error Accept(SafeHandle::Ptr socket, byte *socketAddress, int *socketAddressLen, int *acceptedFd);

    static Error Shutdown(SafeHandle::Ptr socket, SocketShutdown how);

    static Error GetLingerOption(SafeHandle::Ptr socket, LingerOption *&option);
    static Error GetIPv4MulticastOption(SafeHandle::Ptr socket, MulticastOption multicastOption, IPv4MulticastOption *&option);
    static Error GetIPv6MulticastOption(SafeHandle::Ptr socket, MulticastOption multicastOption, IPv6MulticastOption *&option);

    static bool IsIPv6Disabled();

    static Error Poll(PollEvent *pollEvents, uint eventCount, int timeout, uint *triggered);
    static Error Poll(SafeHandle::Ptr socket, PollEvents events, int timeout, PollEvents &triggered);

    class Fcntl{
    public:
      static int SetIsNonBlocking(SafeHandle::Ptr fd, int isNonBlocking);

    };
  };

}// namespace interop

NET_NAMESPACE_END

#endif//RENDU_NET_NET_INTEROP_SYS_H_
