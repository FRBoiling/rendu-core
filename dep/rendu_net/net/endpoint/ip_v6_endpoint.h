/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_IP_V6_ENDPOINT_H
#define RENDU_IP_V6_ENDPOINT_H

#include "address/ip_v6_address.h"

RD_NAMESPACE_BEGIN

  class IPv6EndPoint {
  public:

    // Construct to [::]:0
    IPv6EndPoint() noexcept
      : m_address(), m_port(0) {}

    explicit IPv6EndPoint(IPv6Address address, std::uint16_t port = 0) noexcept
      : m_address(address), m_port(port) {}

  public:
    [[nodiscard]] const IPv6Address &GetAddress() const noexcept { return m_address; }

    [[nodiscard]] uint16 GetPort() const noexcept { return m_port; }

  public:
    static std::optional<IPv6EndPoint> FromString(std::string_view host) noexcept;

    static std::optional<IPv6EndPoint> FromIpPort(std::string_view ip, uint16_t port) noexcept;

    string ToString() const;

  private:

    IPv6Address m_address;
    uint16 m_port;

  };

  inline bool operator==(const IPv6EndPoint &a, const IPv6EndPoint &b) {
    return a.GetAddress() == b.GetAddress() &&
           a.GetPort() == b.GetPort();
  }

  inline bool operator!=(const IPv6EndPoint &a, const IPv6EndPoint &b) {
    return !(a == b);
  }

  inline bool operator<(const IPv6EndPoint &a, const IPv6EndPoint &b) {
    return a.GetAddress() < b.GetAddress() ||
           (a.GetAddress() == b.GetAddress() && a.GetPort() < b.GetPort());
  }

  inline bool operator>(const IPv6EndPoint &a, const IPv6EndPoint &b) {
    return b < a;
  }

  inline bool operator<=(const IPv6EndPoint &a, const IPv6EndPoint &b) {
    return !(b < a);
  }

  inline bool operator>=(const IPv6EndPoint &a, const IPv6EndPoint &b) {
    return !(a < b);
  }

RD_NAMESPACE_END

#endif //RENDU_IP_V6_ENDPOINT_H
