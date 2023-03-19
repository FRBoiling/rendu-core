/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_CORE_FRAMEWORK_SYSTEM_H_
#define RENDU_CORE_CORE_FRAMEWORK_SYSTEM_H_

#include <entt/entt.hpp>
namespace rendu {

class system {
 public:
  virtual void run(entt::registry &registry) = 0;
};

}

#endif //RENDU_CORE_CORE_FRAMEWORK_SYSTEM_H_
