/*
* Created by boil on 2023/10/15.
*/

#include "ip_end_point.h"

RD_NAMESPACE_BEGIN

    IPEndPoint::IPEndPoint() noexcept
        : m_family(AddressFamily::InterNetwork), m_ipv4_end_point() {}

    IPEndPoint::IPEndPoint(IPv4EndPoint &endpoint) noexcept
        : m_family(AddressFamily::InterNetwork), m_ipv4_end_point(endpoint) {}

    IPEndPoint::IPEndPoint(IPv6EndPoint &endpoint) noexcept
        : m_family(AddressFamily::InterNetworkV6), m_ipv6_end_point(endpoint) {
    }

    string IPEndPoint::to_string() const {
      return IsIPv4() ? m_ipv4_end_point.to_string() : m_ipv6_end_point.to_string();
    }

    AddressFamily IPEndPoint::GetAddressFamily() const {
      return m_family;
    }

    bool IPEndPoint::IsIPv4() const noexcept { return m_family == AddressFamily::InterNetwork; }

    bool IPEndPoint::IsIPv6() const noexcept { return m_family == AddressFamily::InterNetworkV6; }

    std::optional<IPEndPoint> IPEndPoint::from_string(std::string_view string) noexcept {
      if (auto ipv4 = IPv4EndPoint::from_string(string); ipv4) {
        return *ipv4;
      }

      if (auto ipv6 = IPv4EndPoint::from_string(string); ipv6) {
        return *ipv6;
      }

      return std::nullopt;
    }


RD_NAMESPACE_END

