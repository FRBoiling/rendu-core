/*
* Created by boil on 2023/10/16.
*/

#include "ip_address.h"

COMMON_NAMESPACE_BEGIN

  string IPAddress::ToString() const {
    return IsIpv4() ? m_ipv4.ToString() : m_ipv6.ToString();
  }

  std::optional<IPAddress>
  IPAddress::FromString(std::string_view string) noexcept {
    if (auto ipv4 = IPv4Address::FromString(string); ipv4) {
      return *ipv4;
    }
    if (auto ipv6 = IPv6Address::FromString(string); ipv6) {
      return *ipv6;
    }
    return std::nullopt;
  }

RD_NAMESPACE_END
