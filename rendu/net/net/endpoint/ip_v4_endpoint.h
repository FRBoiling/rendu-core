/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_COMMON_IP_V4_ENDPOINT_H
#define RENDU_COMMON_IP_V4_ENDPOINT_H

#include "../address/ip_v4_address.h"

COMMON_NAMESPACE_BEGIN

  class IPv4EndPoint {
  public:

    // Construct to 0.0.0.0:0
    IPv4EndPoint() noexcept
      : m_address(), m_port(0) {}

    explicit IPv4EndPoint(IPv4Address address, UINT16 port = 0) noexcept
      : m_address(address), m_port(port) {}

  public:

    static std::optional<IPv4EndPoint> FromString(std::string_view host) noexcept;
    static std::optional<IPv4EndPoint>  FromIpPort(std::string_view ip, uint16_t port) noexcept ;

    std::string ToString() const;


    [[nodiscard]] const IPv4Address &GetAddress() const noexcept { return m_address; }

    [[nodiscard]] UINT16 GetPort() const noexcept { return m_port; }



  private:
    IPv4Address m_address;
    UINT16 m_port;
  };

  inline bool operator==(const IPv4EndPoint &a, const IPv4EndPoint &b) {
    return a.GetAddress() == b.GetAddress() &&
           a.GetPort() == b.GetPort();
  }

  inline bool operator!=(const IPv4EndPoint &a, const IPv4EndPoint &b) {
    return !(a == b);
  }

  inline bool operator<(const IPv4EndPoint &a, const IPv4EndPoint &b) {
    return a.GetAddress() < b.GetAddress() ||
           (a.GetAddress() == b.GetAddress() && a.GetPort() < b.GetPort());
  }

  inline bool operator>(const IPv4EndPoint &a, const IPv4EndPoint &b) {
    return b < a;
  }

  inline bool operator<=(const IPv4EndPoint &a, const IPv4EndPoint &b) {
    return !(b < a);
  }

  inline bool operator>=(const IPv4EndPoint &a, const IPv4EndPoint &b) {
    return !(a < b);
  }

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_IP_V4_ENDPOINT_H
