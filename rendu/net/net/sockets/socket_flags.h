/*
* Created by boil on 2024/3/18.
*/

#ifndef RENDU_NET_NET_SOCKETS_SOCKET_FLAGS_H_
#define RENDU_NET_NET_SOCKETS_SOCKET_FLAGS_H_


#include "net_define.h"

NET_NAMESPACE_BEGIN

enum class SocketFlags
{
  /// <summary>Use no flags for this call.</summary>
  None = 0,
  /// <summary>Process out-of-band data.</summary>
  OutOfBand = 1,
  /// <summary>Peek at the incoming message.</summary>
  Peek = 2,
  /// <summary>Send without using routing tables.</summary>
  DontRoute = 4,
  /// <summary>The message was too large to fit into the specified buffer and was truncated.</summary>
  Truncated = 256, // 0x00000100
  /// <summary>Indicates that the control data did not fit into an internal 64-KB buffer and was truncated.</summary>
  ControlDataTruncated = 512, // 0x00000200
  /// <summary>Indicates a broadcast packet.</summary>
  Broadcast = 1024, // 0x00000400
  /// <summary>Indicates a multicast packet.</summary>
  Multicast = 2048, // 0x00000800
  /// <summary>Partial send or receive for message.</summary>
  Partial = 32768, // 0x00008000
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_SOCKET_FLAGS_H_
