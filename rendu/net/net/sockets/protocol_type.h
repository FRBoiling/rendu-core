/*
* Created by boil on 2024/2/4.
*/

#ifndef RENDU_NET_NET_SOCKETS_PROTOCOL_TYPE_H_
#define RENDU_NET_NET_SOCKETS_PROTOCOL_TYPE_H_

#include "net_define.h"

NET_NAMESPACE_BEGIN

enum class ProtocolType {
  /// <summary>Unknown protocol.</summary>
  Unknown = -1,// 0xFFFFFFFF
  /// <summary>Internet Protocol.</summary>
  IP = 0,
  /// <summary>IPv6 Hop by Hop Options header.</summary>
  IPv6HopByHopOptions = 0,
  /// <summary>Unspecified protocol.</summary>
  Unspecified = 0,
  /// <summary>Internet Control Message Protocol.</summary>
  Icmp = 1,
  /// <summary>Internet Group Management Protocol.</summary>
  Igmp = 2,
  /// <summary>Gateway To Gateway Protocol.</summary>
  Ggp = 3,
  /// <summary>Internet Protocol version 4.</summary>
  IPv4 = 4,
  /// <summary>Transmission Control Protocol.</summary>
  Tcp = 6,
  /// <summary>PARC Universal Packet Protocol.</summary>
  Pup = 12,// 0x0000000C
  /// <summary>User Datagram Protocol.</summary>
  Udp = 17,// 0x00000011
  /// <summary>Internet Datagram Protocol.</summary>
  Idp = 22,// 0x00000016
  /// <summary>Internet Protocol version 6 (IPv6).</summary>
  IPv6 = 41,// 0x00000029
  /// <summary>IPv6 Routing header.</summary>
  IPv6RoutingHeader = 43,// 0x0000002B
  /// <summary>IPv6 Fragment header.</summary>
  IPv6FragmentHeader = 44,// 0x0000002C
  /// <summary>IPv6 Encapsulating Security Payload header.</summary>
  IPSecEncapsulatingSecurityPayload = 50,// 0x00000032
  /// <summary>IPv6 Authentication header. For details, see RFC 2292 section 2.2.1, available at https://www.ietf.org.</summary>
  IPSecAuthenticationHeader = 51,// 0x00000033
  /// <summary>Internet Control Message Protocol for IPv6.</summary>
  IcmpV6 = 58,// 0x0000003A
  /// <summary>IPv6 No next header.</summary>
  IPv6NoNextHeader = 59,// 0x0000003B
  /// <summary>IPv6 Destination Options header.</summary>
  IPv6DestinationOptions = 60,// 0x0000003C
  /// <summary>Net Disk Protocol (unofficial).</summary>
  ND = 77,// 0x0000004D
  /// <summary>Raw IP packet protocol.</summary>
  Raw = 255,// 0x000000FF
  /// <summary>Internet Packet Exchange Protocol.</summary>
  Ipx = 1000,// 0x000003E8
  /// <summary>Sequenced Packet Exchange protocol.</summary>
  Spx = 1256,// 0x000004E8
  /// <summary>Sequenced Packet Exchange version 2 protocol.</summary>
  SpxII = 1257,// 0x000004E9
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_PROTOCOL_TYPE_H_
