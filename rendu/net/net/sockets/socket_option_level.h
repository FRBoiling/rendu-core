/*
* Created by boil on 2024/2/19.
*/

#ifndef RENDU_NET_NET_SOCKETS_SOCKET_OPTION_LEVEL_H_
#define RENDU_NET_NET_SOCKETS_SOCKET_OPTION_LEVEL_H_

#include "net_define.h"

NET_NAMESPACE_BEGIN

enum class SocketOptionLevel {
  /// <summary>
  /// <see cref="T:System.Net.Sockets.Socket" /> options apply only to IP sockets.</summary>
  IP = 0,
  /// <summary>
  /// <see cref="T:System.Net.Sockets.Socket" /> options apply only to TCP sockets.</summary>
  Tcp = 6,
  /// <summary>
  /// <see cref="T:System.Net.Sockets.Socket" /> options apply only to UDP sockets.</summary>
  Udp = 17,// 0x00000011
  /// <summary>
  /// <see cref="T:System.Net.Sockets.Socket" /> options apply only to IPv6 sockets.</summary>
  IPv6 = 41,// 0x00000029
  /// <summary>
  /// <see cref="T:System.Net.Sockets.Socket" /> options apply to all sockets.</summary>
  Socket = 65535,// 0x0000FFFF
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_SOCKET_OPTION_LEVEL_H_
