/*
* Created by boil on 2024/2/12.
*/

#ifndef RENDU_NET_NET_ENDPOINT_END_POINT_H_
#define RENDU_NET_NET_ENDPOINT_END_POINT_H_

#include "address/address_family.h"
#include "address/socket_address.h"

NET_NAMESPACE_BEGIN

class EndPoint {
public:
  virtual AddressFamily GetAddressFamily() = 0;

  virtual SocketAddress Serialize() = 0;

  virtual EndPoint* Create(SocketAddress socketAddress) = 0;
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_ENDPOINT_END_POINT_H_
