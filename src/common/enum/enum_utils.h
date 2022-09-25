/*
* Created by boil on 2022/8/20.
*/

#ifndef RENDU_ENUM_UTILS_H_
#define RENDU_ENUM_UTILS_H_

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
  };
  namespace istream_operators {

    template<typename Char, typename Traits, typename E, detail::enable_if_t<E, int> = 0>
    std::basic_istream<Char, Traits> &operator>>(std::basic_istream<Char, Traits> &is, E &value) {
      using D = std::decay_t<E>;

      std::basic_string<Char, Traits> s;
      is >> s;
      if (const auto v = enum_cast<D>(s)) {
        value = *v;
      } else {
        is.setstate(std::basic_ios<Char>::failbit);
      }
      return is;
    }

  } // namespace rendu::istream_operators

//  namespace ostream_operators {

    template<typename Char, typename Traits, typename E, detail::enable_if_t<E, int> = 0>
    std::basic_ostream<Char, Traits> &operator<<(std::basic_ostream<Char, Traits> &os, E value) {
      using D = std::decay_t<E>;
      using U = underlying_type_t<D>;

      if constexpr (detail::supported<D>::value) {
        if (const auto name = enum_flags_name<D>(value); !name.empty()) {
          for (const auto c: name) {
            os.put(c);
          }
          return os;
        }
      }
      return (os << static_cast<U>(value));
    }

    template<typename Char, typename Traits, typename E, detail::enable_if_t<E, int> = 0>
    std::basic_ostream<Char, Traits> &operator<<(std::basic_ostream<Char, Traits> &os, optional<E> value) {
      return value ? (os << *value) : os;
    }

//  } // namespace rendu::ostream_operators

//  namespace iostream_operators {
//
//    using namespace ostream_operators;
//    using namespace istream_operators;
//
//  } // namespace rendu::iostream_operators

  namespace bitwise_operators {

    template<typename E, detail::enable_if_t<E, int> = 0>
    constexpr E operator~(E rhs) noexcept {
      return static_cast<E>(~static_cast<underlying_type_t<E>>(rhs));
    }

    template<typename E, detail::enable_if_t<E, int> = 0>
    constexpr E operator|(E lhs, E rhs) noexcept {
      return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) | static_cast<underlying_type_t<E>>(rhs));
    }

    template<typename E, detail::enable_if_t<E, int> = 0>
    constexpr E operator&(E lhs, E rhs) noexcept {
      return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) & static_cast<underlying_type_t<E>>(rhs));
    }

    template<typename E, detail::enable_if_t<E, int> = 0>
    constexpr E operator^(E lhs, E rhs) noexcept {
      return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) ^ static_cast<underlying_type_t<E>>(rhs));
    }

    template<typename E, detail::enable_if_t<E, int> = 0>
    constexpr E &operator|=(E &lhs, E rhs) noexcept {
      return lhs = (lhs | rhs);
    }

    template<typename E, detail::enable_if_t<E, int> = 0>
    constexpr E &operator&=(E &lhs, E rhs) noexcept {
      return lhs = (lhs & rhs);
    }

    template<typename E, detail::enable_if_t<E, int> = 0>
    constexpr E &operator^=(E &lhs, E rhs) noexcept {
      return lhs = (lhs ^ rhs);
    }

  } // namespace rendu::bitwise_operators
}//namespace rendu

#endif
