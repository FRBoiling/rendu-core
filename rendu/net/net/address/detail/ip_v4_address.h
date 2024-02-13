/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_IP_V4_ADDRESS_H
#define RENDU_IP_V4_ADDRESS_H

#include "net_define.h"

NET_NAMESPACE_BEGIN

namespace detail {

  class IPv4Address : public EqualityComparable<IPv4Address>,
                      public LessThanComparable<IPv4Address> {

  private:
    using bytes_t = uint8_t[4];

  public:
    constexpr IPv4Address();
    explicit constexpr IPv4Address(uint32_t integer);
    explicit constexpr IPv4Address(const uint8_t (&bytes)[4]);
    explicit constexpr IPv4Address(
        uint8_t b0,
        uint8_t b1,
        uint8_t b2,
        uint8_t b3);

  public:
    /// Get the IPv4 unspedified GetAddress 0.0.0.0 (all zeroes).
    static constexpr IPv4Address Any() ;

    /// Get the IPv4 loopback GetAddress 127.0.0.1.
    static constexpr IPv4Address Loopback() ;

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
    static std::optional<IPv4Address> Parse(std::string_view string) noexcept;

    /// Convert the IP GetAddress to dotted decimal notation.
    ///
    /// eg. "12.67.190.23"
    std::string ToString() const;

    std::span< byte> GetBytes()  {
      // 以m_bytes数据的大小的长度创建std::span，其指向的元素被转换为const byte
      return std::span< byte>(reinterpret_cast< byte*>(m_bytes), std::size(m_bytes));
    }

    [[nodiscard]]constexpr uint ToInt() const{
      return uint32_t(m_bytes[0]) << 24 |
             uint32_t(m_bytes[1]) << 16 |
             uint32_t(m_bytes[2]) << 8 |
             uint32_t(m_bytes[3]);
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

    constexpr bool operator<(IPv4Address other) const;

  private:
    alignas(uint32_t) uint8_t m_bytes[4];
  };

  constexpr bool IPv4Address::operator==(IPv4Address other) const {
    return m_bytes[0] == other.m_bytes[0] &&
           m_bytes[1] == other.m_bytes[1] &&
           m_bytes[2] == other.m_bytes[2] &&
           m_bytes[3] == other.m_bytes[3];
  }

  constexpr bool IPv4Address::operator<(IPv4Address other) const {
    return ToInt() < other.ToInt();
  }

  constexpr IPv4Address::IPv4Address()
      : m_bytes{0, 0, 0, 0} {
  }

  constexpr IPv4Address::IPv4Address(uint32_t integer)
      : m_bytes{static_cast<uint8_t>(integer >> 24),
                static_cast<uint8_t>(integer >> 16),
                static_cast<uint8_t>(integer >> 8),
                static_cast<uint8_t>(integer)} {
  }

  constexpr IPv4Address::IPv4Address(const uint8_t (&bytes)[4])
      : m_bytes{bytes[0],
                bytes[1],
                bytes[2],
                bytes[3]} {
  }

  constexpr IPv4Address::IPv4Address(
      uint8_t b0,
      uint8_t b1,
      uint8_t b2,
      uint8_t b3)
      : m_bytes{b0, b1, b2, b3} {
  }

  constexpr IPv4Address IPv4Address::Any() {
    return IPv4Address{};
  }

  constexpr IPv4Address IPv4Address::Loopback() {
    return IPv4Address{127, 0, 0, 0};
  }

}// namespace detail

NET_NAMESPACE_END

#endif//RENDU_IP_V4_ADDRESS_H
