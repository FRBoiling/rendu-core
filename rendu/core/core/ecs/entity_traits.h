/*
* Created by boil on 2023/2/18.
*/

#ifndef RENDU_CORE_ECS_ENTITY_TRAITS_H_
#define RENDU_CORE_ECS_ENTITY_TRAITS_H_

#include "basic_entity_traits.h"

namespace rendu {

/**
 * @brief Entity traits.
 * @tparam Type Type of identifier.
 */
template<typename Type>
struct entity_traits : basic_entity_traits<internal::entity_traits<Type>> {
  /*! @brief Base type. */
  using base_type = basic_entity_traits<internal::entity_traits<Type>>;
  /*! @brief Page size, default is `RD_SPARSE_PAGE`. */
  static constexpr std::size_t page_size = RD_SPARSE_PAGE;
};

/**
 * @copydoc entity_traits<Entity>::to_integral
 * @tparam Entity The value type.
 */
template<typename Entity>
[[nodiscard]] constexpr typename entity_traits<Entity>::entity_type to_integral(const Entity value) noexcept {
  return entity_traits<Entity>::to_integral(value);
}

/**
 * @copydoc entity_traits<Entity>::to_entity
 * @tparam Entity The value type.
 */
template<typename Entity>
[[nodiscard]] constexpr typename entity_traits<Entity>::entity_type to_entity(const Entity value) noexcept {
  return entity_traits<Entity>::to_entity(value);
}

/**
 * @copydoc entity_traits<Entity>::to_version
 * @tparam Entity The value type.
 */
template<typename Entity>
[[nodiscard]] constexpr typename entity_traits<Entity>::version_type to_version(const Entity value) noexcept {
  return entity_traits<Entity>::to_version(value);
}

} // namespace rendu
#endif //RENDU_CORE_ECS_ENTITY_TRAITS_H_