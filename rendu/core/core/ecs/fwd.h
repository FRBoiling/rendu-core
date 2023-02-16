/*
* Created by boil on 2023/2/16.
*/

#ifndef RENDU_CORE_CORE_ECS_FWD_H_
#define RENDU_CORE_CORE_ECS_FWD_H_

#include <cstdint>
#include <memory>
#include <type_traits>
#include "base/type_traits.h"
#include "base/fwd.h"

namespace rendu {

/*! @brief Default entity identifier. */
enum class entity : id_type {};

} //namespace rendu

#endif //RENDU_CORE_CORE_ECS_FWD_H_
