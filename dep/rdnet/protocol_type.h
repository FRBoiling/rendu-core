/*
* Created by boil on 2023/10/17.
*/

#ifndef RENDU_PROTOCOL_TYPE_H
#define RENDU_PROTOCOL_TYPE_H

#include "define.h"

RD_NAMESPACE_BEGIN

    enum class ProtocolType
    {
      IP = 0, // dummy for IP

      IPv6HopByHopOptions = 0,
      Icmp = 1, // control message protocol
      Igmp = 2, // group management protocol
      Ggp = 3, // gateway^2 (deprecated)

      IPv4 = 4,
      Tcp = 6, // tcp
      Pup = 12, // pup
      Udp = 17, // user datagram protocol
      Idp = 22, // xns idp
      IPv6 = 41, // IPv4
      IPv6RoutingHeader = 43, // IPv6RoutingHeader
      IPv6FragmentHeader = 44, // IPv6FragmentHeader
      IPSecEncapsulatingSecurityPayload = 50, // IPSecEncapsulatingSecurityPayload
      IPSecAuthenticationHeader = 51, // IPSecAuthenticationHeader
      IcmpV6 = 58, // IcmpV6
      IPv6NoNextHeader = 59, // IPv6NoNextHeader
      IPv6DestinationOptions = 60, // IPv6DestinationOptions
      ND = 77, // UNOFFICIAL net disk proto
      Raw = 255,  // raw IP packet

      Unspecified = 0,
      Ipx = 1000,
      Spx = 1256,
      SpxII = 1257,

      Unknown = -1, // unknown protocol type
    };

RD_NAMESPACE_END

#endif //RENDU_PROTOCOL_TYPE_H
