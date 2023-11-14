/*
* Created by boil on 2023/10/15.
*/

#include "ip_end_point.h"

COMMON_NAMESPACE_BEGIN

  std::string IPEndPoint::ToString() const {
    return IsIPv4() ? m_ipv4.ToString() : m_ipv6.ToString();
  }

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


  std::optional<IPEndPoint>
  IPEndPoint::FromIpPort(std::string_view ip, uint16_t port) noexcept {
    if (auto ipv4 = IPv4EndPoint::FromIpPort(ip,port); ipv4) {
      return *ipv4;
    }

    if (auto ipv6 = IPv6EndPoint::FromIpPort(ip,port); ipv6) {
      return *ipv6;
    }

    return std::nullopt;
  }


COMMON_NAMESPACE_END

