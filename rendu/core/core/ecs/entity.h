/*
* Created by boil on 2023/2/15.
*/

#ifndef RENDU_CORE_ECS_ENTITY_H_
#define RENDU_CORE_ECS_ENTITY_H_

#include "entity_traits.h"
#include "null_t.h"
#include "tombstone_t.h"

namespace rendu{

/**
 * @brief Compile-time constant for null entities.
 *
 * There exist implicit conversions from this variable to identifiers of any
 * allowed type. Similarly, there exist comparison operators between the null
 * entity and any other identifier.
 */
inline constexpr null_t null{};

/**
 * @brief Compile-time constant for tombstone entities.
 *
 * There exist implicit conversions from this variable to identifiers of any
 * allowed type. Similarly, there exist comparison operators between the
 * tombstone entity and any other identifier.
 */
inline constexpr tombstone_t tombstone{};

} // namespace rendu

#endif //RENDU_CORE_ECS_ENTITY_H_
