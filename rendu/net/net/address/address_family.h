/*
* Created by boil on 2024/2/4.
*/

#ifndef RENDU_NET_NET_TCP_ADDRESS_FAMILY_H_
#define RENDU_NET_NET_TCP_ADDRESS_FAMILY_H_

#include "net_define.h"

NET_NAMESPACE_BEGIN

enum class AddressFamily {
  /// <summary>Unknown address family.</summary>
  Unknown = -1,// 0xFFFFFFFF
  /// <summary>Unspecified address family.</summary>
  Unspecified = 0,
  /// <summary>Unix local to host address.</summary>
  Unix = 1,
  /// <summary>Address for IP version 4.</summary>
  InterNetwork = 2,
  /// <summary>ARPANET IMP address.</summary>
  ImpLink = 3,
  /// <summary>Address for PUP protocols.</summary>
  Pup = 4,
  /// <summary>Address for MIT CHAOS protocols.</summary>
  Chaos = 5,
  /// <summary>IPX or SPX address.</summary>
  Ipx = 6,
  /// <summary>Address for Xerox NS protocols.</summary>
  NS = 6,
  /// <summary>Address for ISO protocols.</summary>
  Iso = 7,
  /// <summary>Address for OSI protocols.</summary>
  Osi = 7,
  /// <summary>European Computer Manufacturers Association (ECMA) address.</summary>
  Ecma = 8,
  /// <summary>Address for Datakit protocols.</summary>
  DataKit = 9,
  /// <summary>Addresses for CCITT protocols, such as X.25.</summary>
  Ccitt = 10,// 0x0000000A
  /// <summary>IBM SNA address.</summary>
  Sna = 11,// 0x0000000B
  /// <summary>DECnet address.</summary>
  DecNet = 12,// 0x0000000C
  /// <summary>Direct data-link interface address.</summary>
  DataLink = 13,// 0x0000000D
  /// <summary>LAT address.</summary>
  Lat = 14,// 0x0000000E
  /// <summary>NSC Hyperchannel address.</summary>
  HyperChannel = 15,// 0x0000000F
  /// <summary>AppleTalk address.</summary>
  AppleTalk = 16,// 0x00000010
  /// <summary>NetBios address.</summary>
  NetBios = 17,// 0x00000011
  /// <summary>VoiceView address.</summary>
  VoiceView = 18,// 0x00000012
  /// <summary>FireFox address.</summary>
  FireFox = 19,// 0x00000013
  /// <summary>Banyan address.</summary>
  Banyan = 21,// 0x00000015
  /// <summary>Native ATM services address.</summary>
  Atm = 22,// 0x00000016
  /// <summary>Address for IP version 6.</summary>
  InterNetworkV6 = 23,// 0x00000017
  /// <summary>Address for Microsoft cluster products.</summary>
  Cluster = 24,// 0x00000018
  /// <summary>IEEE 1284.4 workgroup address.</summary>
  Ieee12844 = 25,// 0x00000019
  /// <summary>IrDA address.</summary>
  Irda = 26,// 0x0000001A
  /// <summary>Address for Network Designers OSI gateway-enabled protocols.</summary>
  NetworkDesigners = 28,// 0x0000001C
  /// <summary>MAX address.</summary>
  Max = 29,// 0x0000001D
  /// <summary>Low-level Packet address.</summary>
  Packet = 65536,// 0x00010000
  /// <summary>Controller Area Network address.</summary>
  ControllerAreaNetwork = 65537,// 0x00010001
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_TCP_ADDRESS_FAMILY_H_
