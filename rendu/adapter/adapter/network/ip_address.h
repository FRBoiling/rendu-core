/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_IP_ADDRESS_H
#define RENDU_IP_ADDRESS_H


#include "define.h"
#include "address_family.h"
#include "ip_v4_address.h"
#include "ip_v6_address.h"

RD_NAMESPACE_BEGIN

    class IpAddress {
    public:

      // Constructs to IPv4 address 0.0.0.0
      IpAddress() noexcept;

      IpAddress(IPv4Address address) noexcept;

      IpAddress(IPv6Address address) noexcept;

    public:

      AddressFamily GetAddressFamily() const {
        return is_ipv4() ? AddressFamily::InterNetwork : AddressFamily::InterNetworkV6;;
      }

      bool is_ipv4() const noexcept { return m_family == AddressFamily::InterNetwork; }

      bool is_ipv6() const noexcept { return m_family == AddressFamily::InterNetworkV6; }

      const IPv4Address &to_ipv4() const;

      const IPv6Address &to_ipv6() const;

      const std::uint8_t *bytes() const noexcept;

      std::string to_string() const;

      static std::optional<IpAddress> from_string(std::string_view string) noexcept;

    public:
      bool operator==(const IpAddress &rhs) const noexcept;

      bool operator!=(const IpAddress &rhs) const noexcept;

      //  Ipv4Address sorts less than Ipv6Address
      bool operator<(const IpAddress &rhs) const noexcept;

      bool operator>(const IpAddress &rhs) const noexcept;

      bool operator<=(const IpAddress &rhs) const noexcept;

      bool operator>=(const IpAddress &rhs) const noexcept;

    private:
      AddressFamily m_family;
      IPv4Address m_ipv4_address;
      IPv6Address m_ipv6_address;
    };

    inline bool IpAddress::operator==(const IpAddress &rhs) const noexcept {
      if (is_ipv4()) {
        return rhs.is_ipv4() && m_ipv4_address == rhs.m_ipv4_address;
      } else {
        return rhs.is_ipv6() && m_ipv6_address == rhs.m_ipv6_address;
      }
    }

    inline bool IpAddress::operator!=(const IpAddress &rhs) const noexcept {
      return !(*this == rhs);
    }

    inline bool IpAddress::operator<(const IpAddress &rhs) const noexcept {
      if (is_ipv4()) {
        return !rhs.is_ipv4() || m_ipv4_address < rhs.m_ipv4_address;
      } else {
        return rhs.is_ipv6() && m_ipv6_address < rhs.m_ipv6_address;
      }
    }

    inline bool IpAddress::operator>(const IpAddress &rhs) const noexcept {
      return rhs < *this;
    }

    inline bool IpAddress::operator<=(const IpAddress &rhs) const noexcept {
      return !(rhs < *this);
    }

    inline bool IpAddress::operator>=(const IpAddress &rhs) const noexcept {
      return !(*this < rhs);
    }




RD_NAMESPACE_END

#endif //RENDU_IP_ADDRESS_H
