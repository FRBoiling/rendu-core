/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_IP_ADDRESS_H
#define RENDU_IP_ADDRESS_H

#include "net_define.h"

#include "address_family.h"

#include "detail/ip_v4_address.h"
#include "detail/ip_v6_address.h"

NET_NAMESPACE_BEGIN

class IPAddress : public EqualityComparable<IPAddress>,
                  public LessThanComparable<IPAddress> {
public:
  constexpr IPAddress() noexcept;

  constexpr IPAddress(detail::IPv4Address &address) noexcept;

  constexpr IPAddress(detail::IPv6Address &address) noexcept;

  constexpr IPAddress(Long new_address) noexcept;
  constexpr IPAddress(std::span<byte> address) noexcept;
  constexpr IPAddress(std::span<byte> address, Long scopeid) noexcept;

public:
  static constexpr IPAddress Any();
  static constexpr IPAddress Loopback();

  static constexpr IPAddress IPv6Any();
  static constexpr IPAddress IPv6Loopback();

  bool IsIPv4() const noexcept { return m_family == family::ipv4; }

  bool IsIPv6() const noexcept { return m_family == family::ipv6; }

  AddressFamily GetFamily();

  constexpr Long GetScopeId() const;
  constexpr uint GetAddress() const;

  std::span<byte> GetAddressBytes();

  bool IsIPv4MappedToIPv6();
  IPAddress *MapToIPv6();

  static std::optional<IPAddress> Parse(const std::string_view ip_string);

  [[nodiscard]] std::string ToString() const;


  [[nodiscard]] const std::uint8_t *ToBytes() const noexcept;

public:
  bool operator==(const IPAddress &rhs) const noexcept;
  bool operator<(const IPAddress &rhs) const noexcept;

private:
  enum class family {
    ipv4,
    ipv6
  };

  family m_family;

  union {
    detail::IPv4Address m_ipv4;
    detail::IPv6Address m_ipv6;
  };
};

constexpr IPAddress::IPAddress() noexcept
    : m_family(family::ipv4), m_ipv4() {}

constexpr IPAddress::IPAddress(detail::IPv4Address &address) noexcept
    : m_family(family::ipv4), m_ipv4(address) {}

constexpr IPAddress::IPAddress(detail::IPv6Address &address) noexcept
    : m_family(family::ipv6), m_ipv6(address) {
}

constexpr IPAddress::IPAddress(Long new_address) noexcept {
}

constexpr IPAddress::IPAddress(std::span<byte> address) noexcept {
}

constexpr IPAddress::IPAddress(std::span<byte> address, Long scopeid) noexcept {
}

inline bool IPAddress::operator==(const IPAddress &rhs) const noexcept {
  if (IsIPv4()) {
    return rhs.IsIPv4() && m_ipv4 == rhs.m_ipv4;
  } else {
    return rhs.IsIPv6() && m_ipv6 == rhs.m_ipv6;
  }
}

inline bool IPAddress::operator<(const IPAddress &rhs) const noexcept {
  if (IsIPv4()) {
    return !rhs.IsIPv4() || m_ipv4 < rhs.m_ipv4;
  } else {
    return rhs.IsIPv6() && m_ipv6 < rhs.m_ipv6;
  }
}

constexpr IPAddress IPAddress::Any() {
  auto address = detail::IPv4Address::Any();
  return IPAddress{address};
}

constexpr IPAddress IPAddress::Loopback() {
  auto address = detail::IPv4Address::Loopback();
  return IPAddress{address};
}

constexpr IPAddress IPAddress::IPv6Any() {
  auto address = detail::IPv6Address::Any();
  return IPAddress{address};
};

constexpr IPAddress IPAddress::IPv6Loopback() {
  auto address = detail::IPv6Address::Loopback();
  return IPAddress{address};
};


constexpr Long IPAddress::GetScopeId() const {
  return IsIPv6() ? m_ipv6.GetScopeId() : 0;
}

constexpr uint IPAddress::GetAddress() const {
  return IsIPv4() ? m_ipv4.ToInt() : 0;
}

NET_NAMESPACE_END

#endif//RENDU_IP_ADDRESS_H
