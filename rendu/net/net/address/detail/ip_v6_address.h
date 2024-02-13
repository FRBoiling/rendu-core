/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_IP_V6_ADDRESS_H
#define RENDU_IP_V6_ADDRESS_H

#include "net_define.h"

NET_NAMESPACE_BEGIN

namespace detail {

  class IPv6Address : public EqualityComparable<IPv6Address>,
                      public LessThanComparable<IPv6Address> {
  private:
    using bytes_t = std::uint8_t[16];

  public:
    constexpr IPv6Address();

    explicit constexpr IPv6Address(
        std::uint64_t subnetPrefix,
        std::uint64_t interfaceIdentifier,
        Long scope_id = 0);

    explicit constexpr IPv6Address(
        std::uint16_t part0,
        std::uint16_t part1,
        std::uint16_t part2,
        std::uint16_t part3,
        std::uint16_t part4,
        std::uint16_t part5,
        std::uint16_t part6,
        std::uint16_t part7,
        Long scope_id = 0);

    explicit constexpr IPv6Address(
        const std::uint16_t (&parts)[8]);

    explicit constexpr IPv6Address(
        const std::uint8_t (&bytes)[16]);

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
    static std::optional<IPv6Address> Parse(std::string_view string) noexcept;

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

    [[nodiscard]] std::span< byte> GetBytes()  {
      // 以m_bytes数据的大小的长度创建std::span，其指向的元素被转换为const byte
      return std::span< byte>(reinterpret_cast< byte*>(m_bytes), std::size(m_bytes));
    }

    /// Get the IPv6 unspedified GetAddress :: (all zeroes).
    static constexpr IPv6Address Any();

    /// Get the IPv6 loopback GetAddress ::1.
    static constexpr IPv6Address Loopback();

    constexpr Long GetScopeId() const;
    constexpr void SetScopeId(Long scope_id);

    constexpr std::uint64_t subnet_prefix() const;

    constexpr std::uint64_t interface_identifier() const;

    constexpr bool operator==(const IPv6Address &other) const;

    constexpr bool operator<(const IPv6Address &other) const;

  private:
    alignas(std::uint64_t) std::uint8_t m_bytes[16];
    Long m_scope_id;
  };


  constexpr IPv6Address::IPv6Address()
      : m_bytes{
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0} {}

  constexpr IPv6Address::IPv6Address(
      std::uint64_t subnetPrefix,
      std::uint64_t interfaceIdentifier,
      Long scopeId)
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
            static_cast<std::uint8_t>(interfaceIdentifier)},
        m_scope_id(scopeId) {}

  constexpr IPv6Address::IPv6Address(
      std::uint16_t part0,
      std::uint16_t part1,
      std::uint16_t part2,
      std::uint16_t part3,
      std::uint16_t part4,
      std::uint16_t part5,
      std::uint16_t part6,
      std::uint16_t part7,
      Long scope_id)
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
            static_cast<std::uint8_t>(part7)},
        m_scope_id(scope_id) {}

  constexpr IPv6Address::IPv6Address(
      const std::uint16_t (&parts)[8])
      : IPv6Address(
            parts[0], parts[1], parts[2], parts[3],
            parts[4], parts[5], parts[6], parts[7]) {}

  constexpr IPv6Address::IPv6Address(const std::uint8_t (&bytes)[16])
      : m_bytes{
            bytes[0], bytes[1], bytes[2], bytes[3],
            bytes[4], bytes[5], bytes[6], bytes[7],
            bytes[8], bytes[9], bytes[10], bytes[11],
            bytes[12], bytes[13], bytes[14], bytes[15]} {}


  constexpr bool IPv6Address::operator==(const IPv6Address &other) const {
    for (int i = 0; i < 16; ++i) {
      if (m_bytes[i] != other.m_bytes[i]) return false;
    }
    if (m_scope_id != other.m_scope_id) {
      return false;
    }
    return true;
  }

  constexpr bool IPv6Address::operator<(const IPv6Address &other) const {
    for (int i = 0; i < 16; ++i) {
      if (m_bytes[i] != other.m_bytes[i])
        return m_bytes[i] < other.m_bytes[i];
    }
    return m_scope_id < other.m_scope_id;
  }

  constexpr IPv6Address IPv6Address::Any() {
    return IPv6Address{};
  }

  constexpr IPv6Address IPv6Address::Loopback() {
    return IPv6Address{0, 0, 0, 0, 0, 0, 0, 1};
  }

  constexpr Long IPv6Address::GetScopeId() const{
    return m_scope_id;
  };

  constexpr void IPv6Address::SetScopeId(Long scope_id){
    m_scope_id = scope_id;
  }

}// namespace detail

NET_NAMESPACE_END

#endif//RENDU_IP_V6_ADDRESS_H
