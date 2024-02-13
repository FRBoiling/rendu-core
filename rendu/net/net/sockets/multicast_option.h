/*
* Created by boil on 2024/2/20.
*/

#ifndef RENDU_NET_NET_SOCKETS_MULTICAST_OPTION_H_
#define RENDU_NET_NET_SOCKETS_MULTICAST_OPTION_H_

#include "net_define.h"
#include "address/ip_address.h"

NET_NAMESPACE_BEGIN
class MulticastOption {

private:
  IPAddress* m_group;
  IPAddress* m_localAddress;
private:
  int _ifIndex;

public:
  MulticastOption(IPAddress& group, IPAddress& mcint,int interfaceIndex):m_group(&group),m_localAddress(&mcint),_ifIndex(interfaceIndex) {
  }
};
NET_NAMESPACE_END


#endif//RENDU_NET_NET_SOCKETS_MULTICAST_OPTION_H_
