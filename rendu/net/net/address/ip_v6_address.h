/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_IP_V6_ADDRESS_H
#define RENDU_IP_V6_ADDRESS_H

#include "net_define.h"

NET_NAMESPACE_BEGIN

  class IPv6Address {
    using bytes_t = std::uint8_t[16];

  public:

    constexpr IPv6Address();

    explicit constexpr IPv6Address(
      std::uint64_t subnetPrefix,
      std::uint64_t interfaceIdentifier);

    constexpr IPv6Address(
      std::uint16_t part0,
      std::uint16_t part1,
      std::uint16_t part2,
      std::uint16_t part3,
      std::uint16_t part4,
      std::uint16_t part5,
      std::uint16_t part6,
      std::uint16_t part7);

    explicit constexpr IPv6Address(
      const std::uint16_t(&parts)[8]);

    explicit constexpr IPv6Address(
      const std::uint8_t(&bytes)[16]);

public:
    /// Parse a string representation of an IPv6 GetAddress.
    ///
    /// \param string
    /// The string to parse.
    /// Must be in ASCII, UTF-8 or Latin-1 encoding.
    ///
    /// \return
    /// The IP GetAddress if successful, otherwise std::nullopt if the string
    /// could not be parsed as an IPv4 GetAddress.
    static std::optional<IPv6Address> FromString(std::string_view string) noexcept;

    /// Convert the IP GetAddress to contracted string form.
    ///
    /// Address is broken up into 16-bit parts, with each part represended in 1-4
    /// lower-case hexadecimal with leading zeroes omitted. Parts are separated
    /// by separated by a ':'. The longest contiguous run of zero parts is contracted
    /// to "::".
    ///
    /// For example:
    /// IPv6Address::unspecified() -> "::"
    /// IPv6Address::loopback() -> "::1"
    /// IPv6Address(0x0011223344556677, 0x8899aabbccddeeff) ->
    ///   "11:2233:4455:6677:8899:aabb:ccdd:eeff"
    /// IPv6Address(0x0102030400000000, 0x003fc447ab991011) ->
    ///   "102:304::3f:c447:ab99:1011"
    std::string ToString() const;

    constexpr const bytes_t &GetBytes() const { return m_bytes; }



    constexpr std::uint64_t subnet_prefix() const;

    constexpr std::uint64_t interface_identifier() const;

    /// Get the IPv6 unspedified GetAddress :: (all zeroes).
    static constexpr IPv6Address unspecified();

    /// Get the IPv6 loopback GetAddress ::1.
    static constexpr IPv6Address loopback();



    constexpr bool operator==(const IPv6Address &other) const;

    constexpr bool operator!=(const IPv6Address &other) const;

    constexpr bool operator<(const IPv6Address &other) const;

    constexpr bool operator>(const IPv6Address &other) const;

    constexpr bool operator<=(const IPv6Address &other) const;

    constexpr bool operator>=(const IPv6Address &other) const;

  private:

    alignas(std::uint64_t) std::uint8_t m_bytes[16];

  };

  constexpr IPv6Address::IPv6Address()
    : m_bytes{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0} {}

  constexpr IPv6Address::IPv6Address(
    std::uint64_t subnetPrefix,
    std::uint64_t interfaceIdentifier)
    : m_bytes{
    static_cast<std::uint8_t>(subnetPrefix >> 56),
    static_cast<std::uint8_t>(subnetPrefix >> 48),
    static_cast<std::uint8_t>(subnetPrefix >> 40),
    static_cast<std::uint8_t>(subnetPrefix >> 32),
    static_cast<std::uint8_t>(subnetPrefix >> 24),
    static_cast<std::uint8_t>(subnetPrefix >> 16),
    static_cast<std::uint8_t>(subnetPrefix >> 8),
    static_cast<std::uint8_t>(subnetPrefix),
    static_cast<std::uint8_t>(interfaceIdentifier >> 56),
    static_cast<std::uint8_t>(interfaceIdentifier >> 48),
    static_cast<std::uint8_t>(interfaceIdentifier >> 40),
    static_cast<std::uint8_t>(interfaceIdentifier >> 32),
    static_cast<std::uint8_t>(interfaceIdentifier >> 24),
    static_cast<std::uint8_t>(interfaceIdentifier >> 16),
    static_cast<std::uint8_t>(interfaceIdentifier >> 8),
    static_cast<std::uint8_t>(interfaceIdentifier)} {}

  constexpr IPv6Address::IPv6Address(
    std::uint16_t part0,
    std::uint16_t part1,
    std::uint16_t part2,
    std::uint16_t part3,
    std::uint16_t part4,
    std::uint16_t part5,
    std::uint16_t part6,
    std::uint16_t part7)
    : m_bytes{
    static_cast<std::uint8_t>(part0 >> 8),
    static_cast<std::uint8_t>(part0),
    static_cast<std::uint8_t>(part1 >> 8),
    static_cast<std::uint8_t>(part1),
    static_cast<std::uint8_t>(part2 >> 8),
    static_cast<std::uint8_t>(part2),
    static_cast<std::uint8_t>(part3 >> 8),
    static_cast<std::uint8_t>(part3),
    static_cast<std::uint8_t>(part4 >> 8),
    static_cast<std::uint8_t>(part4),
    static_cast<std::uint8_t>(part5 >> 8),
    static_cast<std::uint8_t>(part5),
    static_cast<std::uint8_t>(part6 >> 8),
    static_cast<std::uint8_t>(part6),
    static_cast<std::uint8_t>(part7 >> 8),
    static_cast<std::uint8_t>(part7)} {}

  constexpr IPv6Address::IPv6Address(
    const std::uint16_t(&parts)[8])
    : IPv6Address(
    parts[0], parts[1], parts[2], parts[3],
    parts[4], parts[5], parts[6], parts[7]) {}

  constexpr IPv6Address::IPv6Address(const std::uint8_t(&bytes)[16])
    : m_bytes{
    bytes[0], bytes[1], bytes[2], bytes[3],
    bytes[4], bytes[5], bytes[6], bytes[7],
    bytes[8], bytes[9], bytes[10], bytes[11],
    bytes[12], bytes[13], bytes[14], bytes[15]} {}

  constexpr std::uint64_t IPv6Address::subnet_prefix() const {
    return
      static_cast<std::uint64_t>(m_bytes[0]) << 56 |
      static_cast<std::uint64_t>(m_bytes[1]) << 48 |
      static_cast<std::uint64_t>(m_bytes[2]) << 40 |
      static_cast<std::uint64_t>(m_bytes[3]) << 32 |
      static_cast<std::uint64_t>(m_bytes[4]) << 24 |
      static_cast<std::uint64_t>(m_bytes[5]) << 16 |
      static_cast<std::uint64_t>(m_bytes[6]) << 8 |
      static_cast<std::uint64_t>(m_bytes[7]);
  }

  constexpr std::uint64_t IPv6Address::interface_identifier() const {
    return
      static_cast<std::uint64_t>(m_bytes[8]) << 56 |
      static_cast<std::uint64_t>(m_bytes[9]) << 48 |
      static_cast<std::uint64_t>(m_bytes[10]) << 40 |
      static_cast<std::uint64_t>(m_bytes[11]) << 32 |
      static_cast<std::uint64_t>(m_bytes[12]) << 24 |
      static_cast<std::uint64_t>(m_bytes[13]) << 16 |
      static_cast<std::uint64_t>(m_bytes[14]) << 8 |
      static_cast<std::uint64_t>(m_bytes[15]);
  }

  constexpr IPv6Address IPv6Address::unspecified() {
    return IPv6Address{};
  }

  constexpr IPv6Address IPv6Address::loopback() {
    return IPv6Address{0, 0, 0, 0, 0, 0, 0, 1};
  }

  constexpr bool IPv6Address::operator==(const IPv6Address &other) const {
    for (int i = 0; i < 16; ++i) {
      if (m_bytes[i] != other.m_bytes[i]) return false;
    }
    return true;
  }

  constexpr bool IPv6Address::operator!=(const IPv6Address &other) const {
    return !(*this == other);
  }

  constexpr bool IPv6Address::operator<(const IPv6Address &other) const {
    for (int i = 0; i < 16; ++i) {
      if (m_bytes[i] != other.m_bytes[i])
        return m_bytes[i] < other.m_bytes[i];
    }

    return false;
  }

  constexpr bool IPv6Address::operator>(const IPv6Address &other) const {
    return (other < *this);
  }

  constexpr bool IPv6Address::operator<=(const IPv6Address &other) const {
    return !(other < *this);
  }

  constexpr bool IPv6Address::operator>=(const IPv6Address &other) const {
    return !(*this < other);
  }

NET_NAMESPACE_END

#endif //RENDU_IP_V6_ADDRESS_H
