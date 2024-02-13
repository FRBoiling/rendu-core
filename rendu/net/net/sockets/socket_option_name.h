/*
* Created by boil on 2024/2/19.
*/

#ifndef RENDU_NET_NET_SOCKETS_SOCKET_OPTION_NAME_H_
#define RENDU_NET_NET_SOCKETS_SOCKET_OPTION_NAME_H_

#include "net_define.h"

NET_NAMESPACE_BEGIN

enum class SocketOptionName {
  /// <summary>Close the socket gracefully without lingering.</summary>
  DontLinger = -129,// 0xFFFFFF7F
  /// <summary>Enables a socket to be bound for exclusive access.</summary>
  ExclusiveAddressUse = -5,// 0xFFFFFFFB
  /// <summary>Specifies the IP options to be inserted into outgoing datagrams.</summary>
  IPOptions = 1,
  /// <summary>Send UDP datagrams with checksum set to zero.</summary>
  NoChecksum = 1,
  /// <summary>Disables the Nagle algorithm for send coalescing.</summary>
  NoDelay = 1,
  /// <summary>The socket is listening.</summary>
  AcceptConnection = 2,
  /// <summary>Use urgent data as defined in RFC-1222. This option can be set only once; after it is set, it cannot be turned off.</summary>
  BsdUrgent = 2,
  /// <summary>Use expedited data as defined in RFC-1222. This option can be set only once; after it is set, it cannot be turned off.</summary>
  Expedited = 2,
  /// <summary>Indicates that the application provides the IP header for outgoing datagrams.</summary>
  HeaderIncluded = 2,
  /// <summary>The number of seconds a TCP connection will remain alive/idle before keepalive probes are sent to the remote.</summary>
  TcpKeepAliveTime = 3,
  /// <summary>Change the IP header type of the service field.</summary>
  TypeOfService = 3,
  /// <summary>Set the IP header Time-to-Live field.</summary>
  IpTimeToLive = 4,
  /// <summary>Allows the socket to be bound to an address that is already in use.</summary>
  ReuseAddress = 4,
  /// <summary>Use keep-alives.</summary>
  KeepAlive = 8,
  /// <summary>Set the interface for outgoing multicast packets.</summary>
  MulticastInterface = 9,
  /// <summary>An IP multicast Time to Live.</summary>
  MulticastTimeToLive = 10,// 0x0000000A
  /// <summary>An IP multicast loopback.</summary>
  MulticastLoopback = 11,// 0x0000000B
  /// <summary>Add an IP group membership.</summary>
  AddMembership = 12,// 0x0000000C
  /// <summary>Drop an IP group membership.</summary>
  DropMembership = 13,// 0x0000000D
  /// <summary>Do not fragment IP datagrams.</summary>
  DontFragment = 14,// 0x0000000E
  /// <summary>Join a source group.</summary>
  AddSourceMembership = 15,// 0x0000000F
  /// <summary>Do not route; send the packet directly to the interface addresses.</summary>
  DontRoute = 16,// 0x00000010
  /// <summary>Drop a source group.</summary>
  DropSourceMembership = 16,// 0x00000010
  /// <summary>The number of TCP keep alive probes that will be sent before the connection is terminated.</summary>
  TcpKeepAliveRetryCount = 16,// 0x00000010
  /// <summary>Block data from a source.</summary>
  BlockSource = 17,// 0x00000011
  /// <summary>The number of seconds a TCP connection will wait for a keepalive response before sending another keepalive probe.</summary>
  TcpKeepAliveInterval = 17,// 0x00000011
  /// <summary>Unblock a previously blocked source.</summary>
  UnblockSource = 18,// 0x00000012
  /// <summary>Return information about received packets.</summary>
  PacketInformation = 19,// 0x00000013
  /// <summary>Set or get the UDP checksum coverage.</summary>
  ChecksumCoverage = 20,// 0x00000014
  /// <summary>Specifies the maximum number of router hops for an Internet Protocol version 6 (IPv6) packet. This is similar to Time to Live (TTL) for Internet Protocol version 4.</summary>
  HopLimit = 21,// 0x00000015
  /// <summary>Enables restriction of a IPv6 socket to a specified scope, such as addresses with the same link local or site local prefix. This socket option enables applications to place access restrictions on IPv6 sockets. Such restrictions enable an application running on a private LAN to simply and robustly harden itself against external attacks. This socket option widens or narrows the scope of a listening socket, enabling unrestricted access from public and private users when appropriate, or restricting access only to the same site, as required. This socket option has defined protection levels specified in the <see cref="T:System.Net.Sockets.IPProtectionLevel" /> enumeration.</summary>
  IPProtectionLevel = 23,// 0x00000017
  /// <summary>Indicates if a socket created for the AF_INET6 address family is restricted to IPv6 communications only. Sockets created for the AF_INET6 address family may be used for both IPv6 and IPv4 communications. Some applications may want to restrict their use of a socket created for the AF_INET6 address family to IPv6 communications only. When this value is non-zero (the default on Windows), a socket created for the AF_INET6 address family can be used to send and receive IPv6 packets only. When this value is zero, a socket created for the AF_INET6 address family can be used to send and receive packets to and from an IPv6 address or an IPv4 address. Note that the ability to interact with an IPv4 address requires the use of IPv4 mapped addresses. This socket option is supported on Windows Vista or later.</summary>
  IPv6Only = 27,// 0x0000001B
  /// <summary>Permit sending broadcast messages on the socket.</summary>
  Broadcast = 32,// 0x00000020
  /// <summary>Bypass hardware when possible.</summary>
  UseLoopback = 64,// 0x00000040
  /// <summary>Linger on close if unsent data is present.</summary>
  Linger = 128,// 0x00000080
  /// <summary>Receives out-of-band data in the normal data stream.</summary>
  OutOfBandInline = 256,// 0x00000100
  /// <summary>Specifies the total per-socket buffer space reserved for sends. This is unrelated to the maximum message size or the size of a TCP window.</summary>
  SendBuffer = 4097,// 0x00001001
  /// <summary>Specifies the total per-socket buffer space reserved for receives. This is unrelated to the maximum message size or the size of a TCP window.</summary>
  ReceiveBuffer = 4098,// 0x00001002
  /// <summary>Specifies the low water mark for <see cref="Overload:System.Net.Sockets.Socket.Send" /> operations.</summary>
  SendLowWater = 4099,// 0x00001003
  /// <summary>Specifies the low water mark for <see cref="Overload:System.Net.Sockets.Socket.Receive" /> operations.</summary>
  ReceiveLowWater = 4100,// 0x00001004
  /// <summary>Send a time-out. This option applies only to synchronous methods; it has no effect on asynchronous methods such as the <see cref="M:System.Net.Sockets.Socket.BeginSend(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)" /> method.</summary>
  SendTimeout = 4101,// 0x00001005
  /// <summary>Receive a time-out. This option applies only to synchronous methods; it has no effect on asynchronous methods such as the <see cref="M:System.Net.Sockets.Socket.BeginSend(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)" /> method.</summary>
  ReceiveTimeout = 4102,// 0x00001006
  /// <summary>Gets the error status and clear.</summary>
  Error = 4103,// 0x00001007
  /// <summary>Gets the socket type.</summary>
  Type = 4104,// 0x00001008
  /// <summary>Indicates that the system should defer ephemeral port allocation for outbound connections. This is equivalent to using the Winsock2 SO_REUSE_UNICASTPORT socket option.</summary>
  ReuseUnicastPort = 12295,// 0x00003007
  /// <summary>Updates an accepted socket's properties by using those of an existing socket. This is equivalent to using the Winsock2 SO_UPDATE_ACCEPT_CONTEXT socket option and is supported only on connection-oriented sockets.</summary>
  UpdateAcceptContext = 28683,// 0x0000700B
  /// <summary>Updates a connected socket's properties by using those of an existing socket. This is equivalent to using the Winsock2 SO_UPDATE_CONNECT_CONTEXT socket option and is supported only on connection-oriented sockets.</summary>
  UpdateConnectContext = 28688,// 0x00007010
  /// <summary>Not supported; will throw a <see cref="T:System.Net.Sockets.SocketException" /> if used.</summary>
  MaxConnections = 2147483647,// 0x7FFFFFFF
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_SOCKET_OPTION_NAME_H_
