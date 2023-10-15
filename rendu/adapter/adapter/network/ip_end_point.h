/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_IP_END_POINT_H
#define RENDU_IP_END_POINT_H

#include "define.h"
#include "ip_v4_endpoint.h"
#include "ip_v6_endpoint.h"
#include "ip_end_point.h"
#include "ip_address.h"

RD_NAMESPACE_BEGIN

    class IPEndPoint {
    public:
      // Constructs to IPv4 end-point 0.0.0.0:0
      IPEndPoint() noexcept;
      IPEndPoint(IpAddress address, int port);

      IPEndPoint(IPv4EndPoint &endpoint) noexcept;

      IPEndPoint(IPv6EndPoint &endpoint) noexcept;

    public:
      static std::optional<IPEndPoint> from_string(std::string_view string) noexcept;

    public:
      [[nodiscard]] AddressFamily GetAddressFamily() const;

      [[nodiscard]] const IPv4EndPoint &ToIPv4() const;

      [[nodiscard]] const IPv6EndPoint &ToIPv6() const;

      [[nodiscard]] const IpAddress GetAddress() const noexcept;

      [[nodiscard]] uint16 GetPort() const noexcept;

      [[nodiscard]] string to_string() const;

      [[nodiscard]] bool IsIPv4() const noexcept;

      [[nodiscard]] bool IsIPv6() const noexcept;

    public:
      bool operator==(const IPEndPoint &rhs) const noexcept;

      bool operator!=(const IPEndPoint &rhs) const noexcept;

      //  IPv4EndPoint sorts less than Ipv6Endpoint
      bool operator<(const IPEndPoint &rhs) const noexcept;

      bool operator>(const IPEndPoint &rhs) const noexcept;

      bool operator<=(const IPEndPoint &rhs) const noexcept;

      bool operator>=(const IPEndPoint &rhs) const noexcept;

    private:
      AddressFamily m_family;
      IPv4EndPoint m_ipv4_end_point;
      IPv6EndPoint m_ipv6_end_point;
    };

    inline const IpAddress IPEndPoint::GetAddress() const noexcept {
      if (IsIPv4()) {
        return m_ipv4_end_point.address();
      } else {
        return m_ipv6_end_point.address();
      }
    }

    inline std::uint16_t IPEndPoint::GetPort() const noexcept {
      return IsIPv4() ? m_ipv4_end_point.port() : m_ipv6_end_point.port();
    }


    inline const IPv4EndPoint &IPEndPoint::ToIPv4() const {
      assert(IsIPv4());
      return m_ipv4_end_point;
    }

    inline const IPv6EndPoint &IPEndPoint::ToIPv6() const {
      assert(IsIPv6());
      return m_ipv6_end_point;
    }

    inline bool IPEndPoint::operator==(const IPEndPoint &rhs) const noexcept {
      if (IsIPv4()) {
        return rhs.IsIPv4() && m_ipv4_end_point == rhs.m_ipv4_end_point;
      } else {
        return rhs.IsIPv6() && m_ipv6_end_point == rhs.m_ipv6_end_point;
      }
    }

    inline bool IPEndPoint::operator!=(const IPEndPoint &rhs) const noexcept {
      return !(*this == rhs);
    }

    inline bool IPEndPoint::operator<(const IPEndPoint &rhs) const noexcept {
      if (IsIPv4()) {
        return !rhs.IsIPv4() || m_ipv4_end_point < rhs.m_ipv4_end_point;
      } else {
        return rhs.IsIPv6() && m_ipv6_end_point < rhs.m_ipv6_end_point;
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

RD_NAMESPACE_END
#endif //RENDU_IP_END_POINT_H
