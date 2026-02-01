#include "core/actor/message_pool.h"

BEGIN_NAMESPACE_CORE

MessagePool& MessagePool::instance() {
    static MessagePool instance;
    return instance;
}

END_NAMESPACE_CORE

