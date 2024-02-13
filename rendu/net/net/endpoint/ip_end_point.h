/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_IP_END_POINT_H
#define RENDU_IP_END_POINT_H

#include "net/net_define.h"

#include "address/address_family.h"
#include "address/ip_address.h"
#include "ip_v4_endpoint.h"
#include "ip_v6_endpoint.h"

NET_NAMESPACE_BEGIN

class IPEndPoint {
public:
  // Constructs to IPv4 end-point 0.0.0.0:0
  IPEndPoint() noexcept;

  IPEndPoint(IPAddress& ip_address,u_short port);

  IPEndPoint(IPv4EndPoint endpoint) noexcept;

  IPEndPoint(IPv6EndPoint endpoint) noexcept;

public:
  static std::optional<IPEndPoint> FromString(std::string_view string) noexcept;

  static std::optional<IPEndPoint> FromIpPort(std::string_view ip, uint16_t port) noexcept;

  [[nodiscard]] std::string ToString() const;


  [[nodiscard]] bool IsIPv4() const noexcept { return m_family == family::ipv4; }

  [[nodiscard]] bool IsIPv6() const noexcept { return m_family == family::ipv6; }

  [[nodiscard]] const IPv4EndPoint &ToIPv4() const;

  [[nodiscard]] const IPv6EndPoint &ToIPv6() const;

  [[nodiscard]] IPAddress GetAddress() const noexcept;

  [[nodiscard]] std::uint16_t GetPort() const noexcept;

  AddressFamily GetAddressFamily();

  bool operator==(const IPEndPoint &rhs) const noexcept;

  bool operator!=(const IPEndPoint &rhs) const noexcept;

  //  IPv4EndPoint sorts less than IPv6EndPoint
  bool operator<(const IPEndPoint &rhs) const noexcept;

  bool operator>(const IPEndPoint &rhs) const noexcept;

  bool operator<=(const IPEndPoint &rhs) const noexcept;

  bool operator>=(const IPEndPoint &rhs) const noexcept;

private:
  enum class family {
    ipv4,
    ipv6
  };

  family m_family;

  union {
    IPv4EndPoint m_ipv4;
    IPv6EndPoint m_ipv6;
  };
};

inline IPEndPoint::IPEndPoint() noexcept
    : m_family(family::ipv4), m_ipv4() {}

inline IPEndPoint::IPEndPoint(IPv4EndPoint endpoint) noexcept
    : m_family(family::ipv4), m_ipv4(endpoint) {}

inline IPEndPoint::IPEndPoint(IPv6EndPoint endpoint) noexcept
    : m_family(family::ipv6), m_ipv6(endpoint) {
}

inline const IPv4EndPoint &IPEndPoint::ToIPv4() const {
  assert(IsIPv4());
  return m_ipv4;
}

inline const IPv6EndPoint &IPEndPoint::ToIPv6() const {
  assert(IsIPv6());
  return m_ipv6;
}

inline IPAddress IPEndPoint::GetAddress() const noexcept {
  if (IsIPv4()) {
    return m_ipv4.GetAddress();
  } else {
    return m_ipv6.GetAddress();
  }
}

inline std::uint16_t IPEndPoint::GetPort() const noexcept {
  return IsIPv4() ? m_ipv4.GetPort() : m_ipv6.GetPort();
}

inline bool IPEndPoint::operator==(const IPEndPoint &rhs) const noexcept {
  if (IsIPv4()) {
    return rhs.IsIPv4() && m_ipv4 == rhs.m_ipv4;
  } else {
    return rhs.IsIPv6() && m_ipv6 == rhs.m_ipv6;
  }
}

inline bool IPEndPoint::operator!=(const IPEndPoint &rhs) const noexcept {
  return !(*this == rhs);
}

inline bool IPEndPoint::operator<(const IPEndPoint &rhs) const noexcept {
  if (IsIPv4()) {
    return !rhs.IsIPv4() || m_ipv4 < rhs.m_ipv4;
  } else {
    return rhs.IsIPv6() && m_ipv6 < rhs.m_ipv6;
  }
}

inline bool IPEndPoint::operator>(const IPEndPoint &rhs) const noexcept {
  return rhs < *this;
}

inline bool IPEndPoint::operator<=(const IPEndPoint &rhs) const noexcept {
  return !(rhs < *this);
}

inline bool IPEndPoint::operator>=(const IPEndPoint &rhs) const noexcept {
  return !(*this < rhs);
}

NET_NAMESPACE_END
#endif//RENDU_IP_END_POINT_H
