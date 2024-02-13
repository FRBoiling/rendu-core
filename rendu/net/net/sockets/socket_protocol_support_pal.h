/*
* Created by boil on 2024/2/21.
*/

#ifndef RENDU_NET_NET_SOCKETS_SOCKET_PROTOCOL_SUPPORT_PAL_H_
#define RENDU_NET_NET_SOCKETS_SOCKET_PROTOCOL_SUPPORT_PAL_H_

#include "net_define.h"

#include "address/address_family.h"
#include "interop/sys.h"

NET_NAMESPACE_BEGIN


class SocketProtocolSupportPal {
public:
  static bool OSSupportsIPv6() {
    return SocketProtocolSupportPal::IsSupported(AddressFamily::InterNetworkV6) && !IsIPv6Disabled();
  }

  static bool OSSupportsIPv4() {
    return SocketProtocolSupportPal::IsSupported(AddressFamily::InterNetwork);
  }

  static bool OSSupportsUnixDomainSockets() {
    return SocketProtocolSupportPal::IsSupported(AddressFamily::Unix);
  }

private:
  static bool IsIPv6Disabled() {
    return interop::Sys::IsIPv6Disabled();
  }

  static bool IsSupported(AddressFamily af) {
    //TODO:BOIL
    //    if (af == AddressFamily::Unix && (OperatingSystem.IsTvOS() || OperatingSystem.IsIOS() && !OperatingSystem.IsMacCatalyst()))
    //      return false;
    int fd = -1;
    interop::Error error = interop::Sys::Socket((int) af, 2, 0, fd);
    return error != interop::Error::RD_EAFNOSUPPORT && error != interop::Error::RD_EPROTONOSUPPORT;
  }
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_SOCKET_PROTOCOL_SUPPORT_PAL_H_
