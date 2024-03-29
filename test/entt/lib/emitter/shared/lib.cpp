#include <entt/core/attribute.h>
#include "../common/types.h"

namespace shared {

ENTT_API void emit(test_emitter &emitter) {
  emitter.publish(event{});
  emitter.publish(message{42});
  emitter.publish(message{3});
}
}