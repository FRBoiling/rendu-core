/*
* Created by boil on 2022/8/20.
*/

#ifndef RENDU_ENUM_UTILS_H_
#define RENDU_ENUM_UTILS_H_

#include <iterator>
#include "define.h"
#include "enum.h"

namespace rendu {

  class EnumUtils {
  public:

    template<typename Enum>
    static std::string ToString(Enum value) {
      std::string enum_str;
      enum_str.append(enum_type_name<Enum>());
      enum_str.append("::");
      enum_str.append(enum_flags_name(value));
      return enum_str;
    }

    template<typename Enum>
    static size_t Count() {
      return enum_count<Enum>();
    }

    template<typename Enum>
    static Enum FromIndex(size_t index) {
      return enum_value<Enum>(index);
    }

    template<typename Enum>
    static auto ToIndex(Enum value) {
      return enum_index(value);
    }

    template<typename Enum>
    static bool IsValid(Enum value) {
      try {
        return enum_contains(value);
      } catch (...) {
        return false;
      }
    }

    template<typename Enum>
    static bool IsValid(std::underlying_type_t<Enum> value) {
      return IsValid(static_cast<Enum>(value));
    }
  };

}//namespace rendu

#endif
