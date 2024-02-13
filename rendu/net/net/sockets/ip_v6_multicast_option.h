/*
* Created by boil on 2024/2/20.
*/

#ifndef RENDU_NET_NET_SOCKETS_IP_V_6_MULTICAST_OPTION_H_
#define RENDU_NET_NET_SOCKETS_IP_V_6_MULTICAST_OPTION_H_

#include "address/ip_address.h"
#include "net_define.h"

NET_NAMESPACE_BEGIN

class IPv6MulticastOption {

private:
  IPAddress *m_group;
  Long m_interface;

public:
  IPv6MulticastOption(IPAddress &group, Long ifindex) : m_group(&group), m_interface(ifindex) {
  }

};
NET_NAMESPACE_END


#endif//RENDU_NET_NET_SOCKETS_IP_V_6_MULTICAST_OPTION_H_
