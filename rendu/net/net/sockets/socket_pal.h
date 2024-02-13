/*
* Created by boil on 2024/2/14.
*/

#ifndef RENDU_NET_NET_SOCKETS_SOCKET_PAL_H_
#define RENDU_NET_NET_SOCKETS_SOCKET_PAL_H_

#include "net_define.h"

#include "interop/sys.h"

#include "socket_type.h"

#include "ip_v6_multicast_option.h"
#include "linger_option.h"
#include "multicast_option.h"
#include "safe_socket_handle.h"

NET_NAMESPACE_BEGIN

class SocketPal {

public:
  static SafeSocketHandle::Ptr CreateSocket(int fd);
  static SocketError CreateSocket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType, SafeSocketHandle::Ptr& socket);

  static SocketError SetSockOpt(SafeSocketHandle::Ptr socket, SocketOptionLevel optionLevel, SocketOptionName optionName, std::span<byte> optionValue);
  static SocketError SetSockOpt(SafeSocketHandle::Ptr socket, SocketOptionLevel optionLevel, SocketOptionName optionName, int optionValue);

  static SocketError GetSockOpt(SafeSocketHandle::Ptr socket, SocketOptionLevel optionLevel, SocketOptionName optionName, int &optionValue);
  static SocketError GetSocketErrorForErrorCode(interop::Error error_code);

  static SocketError GetErrorAndTrackSetting(SafeSocketHandle::Ptr socket, SocketOptionLevel optionLevel, SocketOptionName optionName, interop::Error err);

  static SocketError GetSockName(SafeSocketHandle::Ptr socket, byte *addr, int &addr_len);
  static SocketError GetPeerName(SafeSocketHandle::Ptr socket, byte *addr, int &addr_len);

  static SocketError Bind(SafeSocketHandle::Ptr socket, ProtocolType socketProtocolType, std::span<byte> buffer);
  static SocketError Connect(SafeSocketHandle::Ptr socket, Memory<byte> socketAddress);

  static SocketError Listen(SafeSocketHandle::Ptr socket, int backlog);
  static SocketError Accept(SafeSocketHandle::Ptr listenSocket, Memory<byte> &&socketAddress, int &socketAddressLen, SafeSocketHandle::Ptr socket);

  static SocketError GetLingerOption(SafeSocketHandle::Ptr socket, LingerOption *&optionValue);
  static SocketError GetMulticastOption(SafeSocketHandle::Ptr socket, SocketOptionName optionName, MulticastOption *&optionValue);
  static SocketError GetIPv6MulticastOption(SafeSocketHandle::Ptr socket, SocketOptionName optionName, IPv6MulticastOption *&optionValue);

  static bool TryStartConnect(SafeSocketHandle::Ptr socket, Memory<byte>& socketAddress, SocketError& errorCode);
  static bool TryCompleteAccept(SafeSocketHandle::Ptr listen_socket, Memory<byte>& socketAddress, int& socketAddressLen, int& acceptedFd, SocketError& errorCode);
  static bool TryCompleteConnect(SafeSocketHandle::Ptr socket, SocketError& errorCode);

};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_SOCKET_PAL_H_
