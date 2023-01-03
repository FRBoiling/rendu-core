/*
* Created by boil on 2022/10/23.
*/

#ifndef RENDU_MAIN_H_
#define RENDU_MAIN_H_

#include <iostream>

#include <magic_enum.hpp>
#include "../../rendu/common/log/logger.h"

enum class Color : int { RED = -10, BLUE = 0, GREEN = 10 };

std::string_view EnumToString(const Color &c1);

template <typename E>
auto to_integer(magic_enum::Enum<E> value) {
  // magic_enum::Enum<E> - C++17 Concept for enum type.
  return static_cast<magic_enum::underlying_type_t<E>>(value);
}

int main() {
  // Enum variable to string name.
  Color c1 = Color::RED;
  std::string_view c1_name = EnumToString(c1);

  std::cout << c1_name << std::endl; // RED


  // String enum name sequence.
  constexpr auto names = magic_enum::enum_names<Color>();
  std::cout << "Color names:";
  for (const auto& n : names) {
    std::cout << " " << n;
  }
  std::cout << std::endl;
  // Color names: RED BLUE GREEN

  // String name to enum value.
  auto c2 = magic_enum::enum_cast<Color>("BLUE");
  if (c2.has_value()) {
    std::cout << "BLUE = " << to_integer(c2.value()) << std::endl; // BLUE = 0
  }

  // Integer value to enum value.
  auto c3 = magic_enum::enum_cast<Color>(10);
  if (c3.has_value()) {
    std::cout << "GREEN = " << magic_enum::enum_integer(c3.value()) << std::endl; // GREEN = 10
  }

  // Enum value to integer value.
  auto c4_integer = magic_enum::enum_integer(Color::RED);
  std::cout << "RED = " << c4_integer << std::endl; // RED = -10

  using namespace magic_enum::ostream_operators; // out-of-the-box ostream operator for all enums.
  // Ostream operator for enum.
  std::cout << "Color: " << c1 << " " << c2 << " " << c3 << std::endl; // Color: RED BLUE GREEN

  // Number of enum values.
  std::cout << "Color enum size: " << magic_enum::enum_count<Color>() << std::endl; // Color size: 3

  // Indexed access to enum value.
  std::cout << "Color[0] = " << magic_enum::enum_value<Color>(0) << std::endl; // Color[0] = RED

  // Enum value sequence.
  constexpr auto values = magic_enum::enum_values<Color>();
  std::cout << "Colors values:";
  for (const auto c : values) {
    std::cout << " " << c; // Ostream operator for enum.
  }
  std::cout << std::endl;
  // Color sequence: RED BLUE GREEN

  enum class Flags { A = 1, B = 2, C = 4, D = 8 };
  using namespace magic_enum::bitwise_operators; // out-of-the-box bitwise operators for all enums.
  // Support operators: ~, |, &, ^, |=, &=, ^=.
  Flags flag = Flags::A | Flags::C;
  std::cout << flag << std::endl; // 5

  enum color { red, green, blue };

  // Checks whether type is an Unscoped enumeration.
  static_assert(magic_enum::is_unscoped_enum_v<color>);
  static_assert(!magic_enum::is_unscoped_enum_v<Color>);
  static_assert(!magic_enum::is_unscoped_enum_v<Flags>);

  // Checks whether type is an Scoped enumeration.
  static_assert(!magic_enum::is_scoped_enum_v<color>);
  static_assert(magic_enum::is_scoped_enum_v<Color>);
  static_assert(magic_enum::is_scoped_enum_v<Flags>);

  // Enum pair (value enum, string enum name) sequence.
  constexpr auto entries = magic_enum::enum_entries<Color>();
  std::cout << "Colors entries:";
  for (const auto& e : entries) {
    std::cout << " "  << e.second << " = " << static_cast<int>(e.first);
  }
  std::cout << std::endl;
  // Color entries: RED = -10 BLUE = 0 GREEN = 10

  return 0;
}

std::string_view EnumToString(const Color &c1) {
  std::string_view c1_name = magic_enum::enum_name(c1);
  RD_LOG_INFO("Color----- {}", c1_name);
  return c1_name;
}


#endif //RENDU_MAIN_H_