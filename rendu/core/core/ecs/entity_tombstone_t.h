/*
* Created by boil on 2023/2/18.
*/

#ifndef RENDU_CORE_ECS_TOMBSTONE_T_H_
#define RENDU_CORE_ECS_TOMBSTONE_T_H_

#include "entity_traits.h"

namespace rendu {
/*! @brief 所有标识符的逻辑删除对象.  */
struct entity_tombstone_t {
  /**
   * @brief Converts the tombstone object to identifiers of any type.
   * @tparam Entity Type of identifier.
   * @return The tombstone representation for the given type.
   */
  template<typename Entity>
  [[nodiscard]] constexpr operator Entity() const noexcept {
    using traits_type = entity_traits<Entity>;
    constexpr auto value = traits_type::construct(traits_type::entity_mask, traits_type::version_mask);
    return value;
  }

  /**
   * @brief Compares two tombstone objects.
   * @param other A tombstone object.
   * @return True in all cases.
   */
  [[nodiscard]] constexpr bool operator==([[maybe_unused]] const entity_tombstone_t other) const noexcept {
    return true;
  }

  /**
   * @brief Compares two tombstone objects.
   * @param other A tombstone object.
   * @return False in all cases.
   */
  [[nodiscard]] constexpr bool operator!=([[maybe_unused]] const entity_tombstone_t other) const noexcept {
    return false;
  }

  /**
   * @brief Compares a tombstone object and an identifier of any type.
   * @tparam Entity Type of identifier.
   * @param entity Identifier with which to compare.
   * @return False if the two elements differ, true otherwise.
   */
  template<typename Entity>
  [[nodiscard]] constexpr bool operator==(const Entity entity) const noexcept {
    using traits_type = entity_traits<Entity>;
    return traits_type::to_version(entity) == traits_type::to_version(*this);
  }

  /**
   * @brief Compares a tombstone object and an identifier of any type.
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
 * @brief Compares a tombstone object and an identifier of any type.
 * @tparam Entity Type of identifier.
 * @param entity Identifier with which to compare.
 * @param other A tombstone object yet to be converted.
 * @return False if the two elements differ, true otherwise.
 */
template<typename Entity>
[[nodiscard]] constexpr bool operator==(const Entity entity, const entity_tombstone_t other) noexcept {
  return other.operator==(entity);
}

/**
 * @brief Compares a tombstone object and an identifier of any type.
 * @tparam Entity Type of identifier.
 * @param entity Identifier with which to compare.
 * @param other A tombstone object yet to be converted.
 * @return True if the two elements differ, false otherwise.
 */
template<typename Entity>
[[nodiscard]] constexpr bool operator!=(const Entity entity, const entity_tombstone_t other) noexcept {
  return !(other == entity);
}

/**
 * @brief Compile-time constant for tombstone entities.
 *
 * There exist implicit conversions from this variable to identifiers of any
 * allowed type. Similarly, there exist comparison operators between the
 * tombstone entity and any other identifier.
 */
inline constexpr entity_tombstone_t tombstone{};

}//namespace rendu

#endif //RENDU_CORE_ECS_TOMBSTONE_T_H_
