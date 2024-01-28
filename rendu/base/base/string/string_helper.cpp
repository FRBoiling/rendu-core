/*
* Created by boil on 2023/10/15.
*/

#include "string_helper.h"

RD_NAMESPACE_BEGIN

namespace Digit {

  bool IsDigit(char c) {
    return c >= '0' && c <= '9';
  }

  std::uint8_t ToDigitValue(char c) {
    return static_cast<std::uint8_t>(c - '0');
  }


  std::optional<std::uint8_t> TryParseHexDigit(char c) {
    if (c >= '0' && c <= '9') {
      return static_cast<std::uint8_t>(c - '0');
    } else if (c >= 'a' && c <= 'f') {
      return static_cast<std::uint8_t>(c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
      return static_cast<std::uint8_t>(c - 'A' + 10);
    }

    return std::nullopt;
  }

  char HexToChar(std::uint8_t value) {
    return value < 10 ? static_cast<char>('0' + value) : static_cast<char>('a' + value - 10);
  }

  std::optional<std::uint16_t> StringToPort(std::string_view string) {
    if (string.empty()) return std::nullopt;

    std::uint32_t value = 0;
    for (auto c: string) {
      if (!Digit::IsDigit(c)) return std::nullopt;
      value = value * 10 + Digit::ToDigitValue(c);
      if (value > 0xFFFFu) return std::nullopt;
    }

    return static_cast<std::uint16_t>(value);
  }


}// namespace Digit


RD_NAMESPACE_END
