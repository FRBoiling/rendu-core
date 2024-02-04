/*
* Created by boil on 2023/9/19.
*/

#include "world.h"

CORE_NAMESPACE_BEGIN

World::World() {
}
World::~World() {
}

World &World::Instance() {
  static World object;
  return object;
}


CORE_NAMESPACE_END
