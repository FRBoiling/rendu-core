/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_COMMON_IP_V4_ADDRESS_H
#define RENDU_COMMON_IP_V4_ADDRESS_H

#include "common_define.h"

COMMON_NAMESPACE_BEGIN

  class IPv4Address {
    using bytes_t = std::uint8_t[4];

  public:

    constexpr IPv4Address()
      : m_bytes{0, 0, 0, 0} {}

    explicit constexpr IPv4Address(std::uint32_t integer)
      : m_bytes{
      static_cast<std::uint8_t>(integer >> 24),
      static_cast<std::uint8_t>(integer >> 16),
      static_cast<std::uint8_t>(integer >> 8),
      static_cast<std::uint8_t>(integer)} {}

    explicit constexpr IPv4Address(const std::uint8_t(&bytes)[4])
      : m_bytes{bytes[0], bytes[1], bytes[2], bytes[3]} {}

    explicit constexpr IPv4Address(
      std::uint8_t b0,
      std::uint8_t b1,
      std::uint8_t b2,
      std::uint8_t b3)
      : m_bytes{b0, b1, b2, b3} {}

  public:
    /// Parse a string representation of an IP GetAddress.
    ///
    /// Parses strings of the form:
    /// - "num.num.num.num" where num is an integer in range [0, 255].
    /// - A single integer value in range [0, 2^32).
    ///
    /// \param string
    /// The string to parse.
    /// Must be in ASCII, UTF-8 or Latin-1 encoding.
    ///
    /// \return
    /// The IP GetAddress if successful, otherwise std::nullopt if the string
    /// could not be parsed as an IPv4 GetAddress.
    static std::optional<IPv4Address> FromString(std::string_view string) noexcept;

    /// Convert the IP GetAddress to dotted decimal notation.
    ///
    /// eg. "12.67.190.23"
    std::string ToString() const;

    [[nodiscard]] constexpr const bytes_t &GetBytes() const { return m_bytes; }

    [[nodiscard]] constexpr std::uint32_t to_integer() const {
      return
        std::uint32_t(m_bytes[0]) << 24 |
        std::uint32_t(m_bytes[1]) << 16 |
        std::uint32_t(m_bytes[2]) << 8 |
        std::uint32_t(m_bytes[3]);
    }

    static constexpr IPv4Address loopback() {
      return IPv4Address(127, 0, 0, 1);
    }

    [[nodiscard]] constexpr bool is_loopback() const {
      return m_bytes[0] == 127;
    }

    [[nodiscard]] constexpr bool is_private_network() const {
      return m_bytes[0] == 10 ||
             (m_bytes[0] == 172 && (m_bytes[1] & 0xF0) == 0x10) ||
             (m_bytes[0] == 192 && m_bytes[2] == 168);
    }

    constexpr bool operator==(IPv4Address other) const;

    constexpr bool operator!=(IPv4Address other) const;

    constexpr bool operator<(IPv4Address other) const;

    constexpr bool operator>(IPv4Address other) const;

    constexpr bool operator<=(IPv4Address other) const;

    constexpr bool operator>=(IPv4Address other) const;


  private:
    alignas(std::uint32_t) std::uint8_t m_bytes[4];
  };

  constexpr bool IPv4Address::operator==(IPv4Address other) const {
    return
      m_bytes[0] == other.m_bytes[0] &&
      m_bytes[1] == other.m_bytes[1] &&
      m_bytes[2] == other.m_bytes[2] &&
      m_bytes[3] == other.m_bytes[3];
  }

  constexpr bool IPv4Address::operator!=(IPv4Address other) const {
    return !(*this == other);
  }

  constexpr bool IPv4Address::operator<(IPv4Address other) const {
    return to_integer() < other.to_integer();
  }

  constexpr bool IPv4Address::operator>(IPv4Address other) const {
    return other < *this;
  }

  constexpr bool IPv4Address::operator<=(IPv4Address other) const {
    return !(other < *this);
  }

  constexpr bool IPv4Address::operator>=(IPv4Address other) const {
    return !(*this < other);
  }

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_IP_V4_ADDRESS_H
