/*
* Created by boil on 2023/10/16.
*/

#include "ip_address.h"

RD_NAMESPACE_BEGIN

    IpAddress::IpAddress() noexcept
        : m_family(AddressFamily::InterNetwork), m_ipv4_address() {}

    IpAddress::IpAddress(IPv4Address address) noexcept
        : m_family(AddressFamily::InterNetwork), m_ipv4_address(address) {}

    IpAddress::IpAddress(IPv6Address address) noexcept
        : m_family(AddressFamily::InterNetworkV6), m_ipv6_address(address) {
    }

    const IPv4Address &IpAddress::to_ipv4() const {
      ASSERT(is_ipv4());
      return m_ipv4_address;
    }

    const IPv6Address &IpAddress::to_ipv6() const {
      ASSERT(is_ipv6());
      return m_ipv6_address;
    }

    const std::uint8_t *IpAddress::bytes() const noexcept {
      return is_ipv4() ? m_ipv4_address.bytes() : m_ipv6_address.bytes();
    }

    std::string IpAddress::to_string() const {
      return is_ipv4() ? m_ipv4_address.to_string() : m_ipv6_address.to_string();
    }

RD_NAMESPACE_END

