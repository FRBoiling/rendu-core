/*
* Created by boil on 2023/10/15.
*/

#include "ip_end_point.h"

RD_NAMESPACE_BEGIN

  std::optional<IPEndPoint>
  IPEndPoint::FromString(std::string_view string) noexcept {
    if (auto ipv4 = IPv4EndPoint::FromString(string); ipv4) {
      return *ipv4;
    }

    if (auto ipv6 = IPv6EndPoint::FromString(string); ipv6) {
      return *ipv6;
    }

    return std::nullopt;
  }

  std::string IPEndPoint::ToString() const {
    return IsIPv4() ? m_ipv4.ToString() : m_ipv6.ToString();
  }

RD_NAMESPACE_END

