/*
* Created by boil on 2023/2/18.
*/

#ifndef RENDU_CORE_ECS_NULL_T_H_
#define RENDU_CORE_ECS_NULL_T_H_

#include "entity_traits.h"

namespace rendu {

/*! @brief Null object for all identifiers.  */
struct null_t {
  /**
   * @brief Converts the null object to identifiers of any type.
   * @tparam Entity Type of identifier.
   * @return The null representation for the given type.
   */
  template<typename Entity>
  [[nodiscard]] constexpr operator Entity() const noexcept {
    using traits_type = entity_traits<Entity>;
    constexpr auto value = traits_type::construct(traits_type::entity_mask, traits_type::version_mask);
    return value;
  }

  /**
   * @brief Compares two null objects.
   * @param other A null object.
   * @return True in all cases.
   */
  [[nodiscard]] constexpr bool operator==([[maybe_unused]] const null_t other) const noexcept {
    return true;
  }

  /**
   * @brief Compares two null objects.
   * @param other A null object.
   * @return False in all cases.
   */
  [[nodiscard]] constexpr bool operator!=([[maybe_unused]] const null_t other) const noexcept {
    return false;
  }

  /**
   * @brief Compares a null object and an identifier of any type.
   * @tparam Entity Type of identifier.
   * @param entity Identifier with which to compare.
   * @return False if the two elements differ, true otherwise.
   */
  template<typename Entity>
  [[nodiscard]] constexpr bool operator==(const Entity entity) const noexcept {
    using traits_type = entity_traits<Entity>;
    return traits_type::to_entity(entity) == traits_type::to_entity(*this);
  }

  /**
   * @brief Compares a null object and an identifier of any type.
   * @tparam Entity Type of identifier.
   * @param entity Identifier with which to compare.
   * @return True if the two elements differ, false otherwise.
   */
  template<typename Entity>
  [[nodiscard]] constexpr bool operator!=(const Entity entity) const noexcept {
    return !(entity == *this);
  }

};

/**
* @brief Compares a null object and an identifier of any type.
* @tparam Entity Type of identifier.
* @param entity Identifier with which to compare.
* @param other A null object yet to be converted.
* @return False if the two elements differ, true otherwise.
*/
template<typename Entity>
[[nodiscard]] constexpr bool operator==(const Entity entity, const null_t other) noexcept {
  return other.operator==(entity);
}

/**
 * @brief Compares a null object and an identifier of any type.
 * @tparam Entity Type of identifier.
 * @param entity Identifier with which to compare.
 * @param other A null object yet to be converted.
 * @return True if the two elements differ, false otherwise.
 */
template<typename Entity>
[[nodiscard]] constexpr bool operator!=(const Entity entity, const null_t other) noexcept {
  return !(other == entity);
}

}//namespace rendu

#endif //RENDU_CORE_ECS_NULL_T_H_
