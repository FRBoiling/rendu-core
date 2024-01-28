/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_IP_ADDRESS_H
#define RENDU_IP_ADDRESS_H

#include "net/net_define.h"

#include "ip_v4_address.h"
#include "ip_v6_address.h"

NET_NAMESPACE_BEGIN

  class IPAddress {
  public:
    // Constructs to IPv4 address 0.0.0.0
    IPAddress() noexcept;

    IPAddress(IPv4Address address) noexcept;

    IPAddress(IPv6Address address) noexcept;

  public:
    static std::optional<IPAddress> FromString(std::string_view string) noexcept;

    [[nodiscard]] std::string ToString() const;

    [[nodiscard]] bool IsIpv4() const noexcept { return m_family == family::ipv4; }

    [[nodiscard]] bool IsIpv6() const noexcept { return m_family == family::ipv6; }

    [[nodiscard]] const IPv4Address &ToIpv4() const;

    [[nodiscard]] const IPv6Address &ToIpv6() const;

    [[nodiscard]] const std::uint8_t *ToBytes() const noexcept;

  public:
    bool operator==(const IPAddress &rhs) const noexcept;

    bool operator!=(const IPAddress &rhs) const noexcept;

    //  IPv4Address sorts less than IPv6Address
    bool operator<(const IPAddress &rhs) const noexcept;

    bool operator>(const IPAddress &rhs) const noexcept;

    bool operator<=(const IPAddress &rhs) const noexcept;

    bool operator>=(const IPAddress &rhs) const noexcept;

  private:

    enum class family {
      ipv4,
      ipv6
    };

    family m_family;

    union {
      IPv4Address m_ipv4;
      IPv6Address m_ipv6;
    };

  };

  inline IPAddress::IPAddress() noexcept
    : m_family(family::ipv4), m_ipv4() {}

  inline IPAddress::IPAddress(IPv4Address address) noexcept
    : m_family(family::ipv4), m_ipv4(address) {}

  inline IPAddress::IPAddress(IPv6Address address) noexcept
    : m_family(family::ipv6), m_ipv6(address) {
  }

  inline const IPv4Address &IPAddress::ToIpv4() const {
    assert(IsIpv4());
    return m_ipv4;
  }

  inline const IPv6Address &IPAddress::ToIpv6() const {
    assert(IsIpv6());
    return m_ipv6;
  }

  inline const std::uint8_t *IPAddress::ToBytes() const noexcept {
    return IsIpv4() ? m_ipv4.GetBytes() : m_ipv6.GetBytes();
  }

  inline bool IPAddress::operator==(const IPAddress &rhs) const noexcept {
    if (IsIpv4()) {
      return rhs.IsIpv4() && m_ipv4 == rhs.m_ipv4;
    } else {
      return rhs.IsIpv6() && m_ipv6 == rhs.m_ipv6;
    }
  }

  inline bool IPAddress::operator!=(const IPAddress &rhs) const noexcept {
    return !(*this == rhs);
  }

  inline bool IPAddress::operator<(const IPAddress &rhs) const noexcept {
    if (IsIpv4()) {
      return !rhs.IsIpv4() || m_ipv4 < rhs.m_ipv4;
    } else {
      return rhs.IsIpv6() && m_ipv6 < rhs.m_ipv6;
    }
  }

  inline bool IPAddress::operator>(const IPAddress &rhs) const noexcept {
    return rhs < *this;
  }

  inline bool IPAddress::operator<=(const IPAddress &rhs) const noexcept {
    return !(rhs < *this);
  }

  inline bool IPAddress::operator>=(const IPAddress &rhs) const noexcept {
    return !(*this < rhs);
  }


NET_NAMESPACE_END

#endif //RENDU_IP_ADDRESS_H
