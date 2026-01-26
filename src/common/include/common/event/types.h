#pragma once

#include "common/define.h"
#include "common/event/event.h"
#include <functional>

BEGIN_NAMESPACE_COMMON
namespace event {

/**
 * @brief 事件处理器函数类型
 */
using EventHandler = std::function<void(const Event&)>;

/**
 * @brief 订阅 ID 类型
 */
using SubscriptionId = uint64_t;

} // namespace event
END_NAMESPACE_COMMON
