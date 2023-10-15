/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_IP_V4_ENDPOINT_H
#define RENDU_IP_V4_ENDPOINT_H

#include "ip_v4_address.h"

RD_NAMESPACE_BEGIN

    class IPv4EndPoint {
    public:

      // Construct to 0.0.0.0:0
      IPv4EndPoint() noexcept
          : m_address(), m_port(0) {}

      explicit IPv4EndPoint(IPv4Address address, uint16 port = 0) noexcept
          : m_address(address), m_port(port) {}

    public:

      const IPv4Address &address() const noexcept { return m_address; }

      uint16 port() const noexcept { return m_port; }

      std::string to_string() const;

      static std::optional<IPv4EndPoint> from_string(std::string_view string) noexcept;

    private:

      IPv4Address m_address;
      uint16 m_port;

    };


    inline bool operator==(const IPv4EndPoint &a, const IPv4EndPoint &b) {
      return a.address() == b.address() &&
             a.port() == b.port();
    }

    inline bool operator!=(const IPv4EndPoint &a, const IPv4EndPoint &b) {
      return !(a == b);
    }

    inline bool operator<(const IPv4EndPoint &a, const IPv4EndPoint &b) {
      return a.address() < b.address() ||
             (a.address() == b.address() && a.port() < b.port());
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

RD_NAMESPACE_END

#endif //RENDU_IP_V4_ENDPOINT_H
