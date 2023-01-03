/*
* Created by boil on 2023/1/2.
*/

#ifndef RENDU_CORE_ENUM_HELPER_H_
#define RENDU_CORE_ENUM_HELPER_H_

#include "magic_enum.hpp"

template<typename E>
std::string_view EnumToString(const E &c1) {
  return magic_enum::enum_name(c1);
}

#endif //RENDU_CORE_ENUM_HELPER_H_
