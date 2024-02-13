/*
* Created by boil on 2023/10/16.
*/

#include "ip_v6_address.h"
#include "net/net_define.h"

NET_NAMESPACE_BEGIN

namespace detail {

  constexpr std::uint64_t IPv6Address::subnet_prefix() const {
    return static_cast<std::uint64_t>(m_bytes[0]) << 56 |
           static_cast<std::uint64_t>(m_bytes[1]) << 48 |
           static_cast<std::uint64_t>(m_bytes[2]) << 40 |
           static_cast<std::uint64_t>(m_bytes[3]) << 32 |
           static_cast<std::uint64_t>(m_bytes[4]) << 24 |
           static_cast<std::uint64_t>(m_bytes[5]) << 16 |
           static_cast<std::uint64_t>(m_bytes[6]) << 8 |
           static_cast<std::uint64_t>(m_bytes[7]);
  }

  constexpr std::uint64_t IPv6Address::interface_identifier() const {
    return static_cast<std::uint64_t>(m_bytes[8]) << 56 |
           static_cast<std::uint64_t>(m_bytes[9]) << 48 |
           static_cast<std::uint64_t>(m_bytes[10]) << 40 |
           static_cast<std::uint64_t>(m_bytes[11]) << 32 |
           static_cast<std::uint64_t>(m_bytes[12]) << 24 |
           static_cast<std::uint64_t>(m_bytes[13]) << 16 |
           static_cast<std::uint64_t>(m_bytes[14]) << 8 |
           static_cast<std::uint64_t>(m_bytes[15]);
  }


  std::optional<IPv6Address> IPv6Address::Parse(std::string_view string) noexcept {
    // Longest possible valid IPv6 string is
    // "xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx%nnn"
    constexpr std::size_t maxLength = 49;

    if (string.empty() || string.length() > maxLength) {
      return std::nullopt;
    }

    // Find % symbol, and get scope id string
    std::size_t percent_pos = string.find('%');
    std::string scope_id_str;
    if (percent_pos != std::string_view::npos) {
      scope_id_str = std::string(string.substr(percent_pos + 1));
      string.remove_suffix(string.length() - percent_pos);
    }

    const std::size_t length = string.length();

    std::optional<int> doubleColonPos;
    std::size_t pos = 0;
    if (length >= 2 && string[0] == ':' && string[1] == ':') {
      doubleColonPos = 0;
      pos = 2;
    }

    int partCount = 0;
    std::uint16_t parts[8] = {0};

    // ... Rest of your code ...

    // Parse scope id, if present
    std::uint32_t scope_id = 0;
    if (!scope_id_str.empty()) {
      try {
        // It might throw an exception if the string is not a valid number or if the number is too large
        scope_id = std::stoul(scope_id_str);
      }
      catch (const std::exception&) {
        return std::nullopt;
      }
    }

    return IPv6Address{parts[0], parts[1], parts[2], parts[3], parts[4], parts[5], parts[6], parts[7], scope_id};
  }

  std::string IPv6Address::ToString() const {
    std::uint32_t longestZeroRunStart = 0;
    std::uint32_t longestZeroRunLength = 0;
    for (std::uint32_t i = 0; i < 8;) {
      if (m_bytes[2 * i] == 0 && m_bytes[2 * i + 1] == 0) {
        const std::uint32_t zeroRunStart = i;
        ++i;
        while (i < 8 && m_bytes[2 * i] == 0 && m_bytes[2 * i + 1] == 0) {
          ++i;
        }

        std::uint32_t zeroRunLength = i - zeroRunStart;
        if (zeroRunLength > longestZeroRunLength) {
          longestZeroRunLength = zeroRunLength;
          longestZeroRunStart = zeroRunStart;
        }
      } else {
        ++i;
      }
    }

    // Longest string will be 8 x 4 digits + 7 ':' separators
    char buffer[40];

    char *c = &buffer[0];

    auto appendPart = [&](std::uint32_t index) {
      const std::uint8_t highByte = m_bytes[index * 2];
      const std::uint8_t lowByte = m_bytes[index * 2 + 1];

      // Don't output leading zero hex digits in the part string.
      if (highByte > 0 || lowByte > 15) {
        if (highByte > 0) {
          if (highByte > 15) {
            *c++ = Digit::HexToChar(highByte >> 4);
          }
          *c++ = Digit::HexToChar(highByte & 0xF);
        }
        *c++ = Digit::HexToChar(lowByte >> 4);
      }
      *c++ = Digit::HexToChar(lowByte & 0xF);
    };

    if (longestZeroRunLength >= 2) {
      for (std::uint32_t i = 0; i < longestZeroRunStart; ++i) {
        if (i > 0) {
          *c++ = ':';
        }

        appendPart(i);
      }

      *c++ = ':';
      *c++ = ':';

      for (std::uint32_t i = longestZeroRunStart + longestZeroRunLength; i < 8; ++i) {
        appendPart(i);

        if (i < 7) {
          *c++ = ':';
        }
      }
    } else {
      appendPart(0);
      for (std::uint32_t i = 1; i < 8; ++i) {
        *c++ = ':';
        appendPart(i);
      }
    }

    assert((c - &buffer[0]) <= sizeof(buffer));

    return std::string{&buffer[0], c};
  }

}// namespace detail

NET_NAMESPACE_END
