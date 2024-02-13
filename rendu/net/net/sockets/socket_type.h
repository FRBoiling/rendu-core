/*
* Created by boil on 2024/2/4.
*/

#ifndef RENDU_NET_NET_TCP_SOCKET_TYPE_H_
#define RENDU_NET_NET_TCP_SOCKET_TYPE_H_

#include "net_define.h"

NET_NAMESPACE_BEGIN

enum class SocketType {
  // 未知类型的 Socket
  Unknown = -1,// 0xFFFFFFFF
  // 支持可靠的、双向的、基于连接的字节流传输，不重复数据，不保留边界。一个 Socket 类型的通信与一个单一的对等体进行通信，需要在通信开始前建立远程主机连接。使用传输控制协议 (TCP) 和 Internet 协议版本 4 (IPv4) 地址家族。
  Stream = 1,
  // 支持基于数据报的连接，是一种无连接的、不可靠的消息，最大长度为固定（通常较小）。消息可能会丢失或重复，并且可能会以错误的顺序到达。一个类型为 Dgram 的 Socket 无需在发送和接收数据之前建立连接，并且可以与多个对等体进行通信。使用用户数据报协议 (UDP) 和 IPv4 地址家族。
  Dgram = 2,
  // 支持对底层传输协议的访问。使用 Dgram ，你可以使用协议，如互联网控制消息协议 (ICMP) 和互联网组管理协议 (IGMP)。你的应用程序必须在发送时提供完整的 IP 头。接收到的数据报将返回完整的 IP 头和选项。
  Raw = 3,
  // 支持无连接的、基于消息的、可靠的、按顺序交付的消息，并且在数据中保留消息边界。Rdm（可靠数据报）消息到达时不会重复，并且发送者会收到丢失消息的通知。如果你使用 SocketType.Rdm 初始化一个 Socket ，则无需在发送和接收数据之前建立远程主机连接。使用 Rdm ，你可以与多个对等体进行通信。
  Rdm = 4,
  // 提供面向连接的和可靠的双向传输的有序字节流。SocketType.Seqpacket 不重复数据，并且在数据流中保留边界。一个类型为 Seqpacket 的 Socket 与一个单一的对等体进行通信，需要在通信开始前建立远程主机连接。
  Seqpacket = 5
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_TCP_SOCKET_TYPE_H_
