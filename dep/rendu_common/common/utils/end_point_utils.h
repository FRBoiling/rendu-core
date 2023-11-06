/*
* Created by boil on 2023/10/17.
*/

#ifndef RENDU_END_POINT_UTILS_H
#define RENDU_END_POINT_UTILS_H

#include "common/define.h"

namespace {
    namespace local {
        constexpr bool is_digit(char c) {
          return c >= '0' && c <= '9';
        }

        constexpr std::uint8_t digit_value(char c) {
          return static_cast<std::uint8_t>(c - '0');
        }

        std::optional<std::uint8_t> try_parse_hex_digit(char c) {
          if (c >= '0' && c <= '9') {
            return static_cast<std::uint8_t>(c - '0');
          } else if (c >= 'a' && c <= 'f') {
            return static_cast<std::uint8_t>(c - 'a' + 10);
          } else if (c >= 'A' && c <= 'F') {
            return static_cast<std::uint8_t>(c - 'A' + 10);
          }

          return std::nullopt;
        }

        char hex_char(std::uint8_t value) {
          return value < 10 ?
                 static_cast<char>('0' + value) :
                 static_cast<char>('a' + value - 10);
        }

        std::optional<std::uint16_t> parse_port(std::string_view string) {
          if (string.empty()) return std::nullopt;

          std::uint32_t value = 0;
          for (auto c: string) {
            if (!is_digit(c)) return std::nullopt;
            value = value * 10 + digit_value(c);
            if (value > 0xFFFFu) return std::nullopt;
          }

          return static_cast<std::uint16_t>(value);
        }
    }
}

RD_NAMESPACE_BEGIN


RD_NAMESPACE_END

#endif //RENDU_END_POINT_UTILS_H
