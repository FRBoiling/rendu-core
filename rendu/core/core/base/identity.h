/*
* Created by boil on 2023/2/20.
*/

#ifndef RENDU_CORE_BASE_IDENTITY_H_
#define RENDU_CORE_BASE_IDENTITY_H_
#include <cstddef>
#include <utility>

namespace rendu{

/*! @brief Identity function object (waiting for C++20). */
struct identity {
  /*! @brief Indicates that this is a transparent function object. */
  using is_transparent = void;

  /**
   * @brief Returns its argument unchanged.
   * @tparam Type Type of the argument.
   * @param value The actual argument.
   * @return The submitted value as-is.
   */
  template<typename Type>
  [[nodiscard]] constexpr Type &&operator()(Type &&value) const noexcept {
    return std::forward<Type>(value);
  }
};

}//namespace rendu

#endif //RENDU_CORE_BASE_IDENTITY_H_
