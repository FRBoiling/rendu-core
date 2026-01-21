# 阶段 6: Common 层 - 事件系统 (event)

## 目标
- 实现事件总线机制
- 支持事件订阅和分发
- 支持同步和异步分发

---

## 文件结构

```
src/common/
├── include/common/event/
│   ├── event.h
│   ├── event_bus.h
│   └── handler.h
└── src/event/
    ├── event.cpp
    ├── event_bus.cpp
    └── handler.cpp
```

---

## 任务清单

### 1. 事件定义 (event.h/cpp)
- [ ] 事件基类
- [ ] 事件类型标识
- [ ] 事件携带数据

### 2. 事件总线 (event_bus.h/cpp)
- [ ] 事件订阅
- [ ] 事件发布
- [ ] 同步分发
- [ ] 异步分发（基于 io_context）
- [ ] 订阅者管理

### 3. 事件处理器 (handler.h/cpp)
- [ ] 处理器接口
- [ ] 支持函数、lambda、std::function
- [ ] 支持优先级（可选）

---

## 头文件设计

### event.h
```cpp
#pragma once

#include <string>
#include <memory>

namespace rendu::event {

class Event {
public:
    Event() = default;
    virtual ~Event() = default;

    // 事件类型
    virtual std::string type() const = 0;
};

// 模板化事件
template<typename T>
class TypedEvent : public Event {
public:
    static std::string type() {
        return T::static_type();
    }

    std::string type() const override {
        return T::static_type();
    }
};

} // namespace rendu::event
```

### event_bus.h
```cpp
#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <common/io/io_context.h>

namespace rendu::event {

class Event;
using EventHandler = std::function<void(const Event&)>;

class EventBus {
public:
    explicit EventBus(IoContext& io);
    ~EventBus();

    // 订阅事件
    template<typename EventType>
    void subscribe(EventHandler handler) {
        subscribe(EventType::type(), std::move(handler));
    }

    void subscribe(const std::string& event_type, EventHandler handler);

    // 取消订阅（返回订阅 ID）
    using SubscriptionId = size_t;
    SubscriptionId subscribe_with_id(const std::string& event_type, EventHandler handler);
    void unsubscribe(SubscriptionId id);

    // 发布事件（同步）
    void publish(const Event& event);

    // 发布事件（异步）
    void publish_async(const Event& event);

private:
    IoContext& io_;
    std::unordered_map<std::string, std::vector<EventHandler>> handlers_;
    std::unordered_map<SubscriptionId, std::string> subscriptions_;
    std::mutex mutex_;
    SubscriptionId next_id_;
};

} // namespace rendu::event
```

### handler.h
```cpp
#pragma once

#include <functional>
#include <common/event/event.h>

namespace rendu::event {

// 便捷的处理器包装器
template<typename EventType>
class Handler {
public:
    using Callback = std::function<void(const EventType&)>;

    explicit Handler(Callback cb)
        : callback_(std::move(cb)) {}

    void operator()(const Event& event) const {
        const EventType* typed = dynamic_cast<const EventType*>(&event);
        if (typed) {
            callback_(*typed);
        }
    }

private:
    Callback callback_;
};

} // namespace rendu::event
```

---

## 单元测试

### 测试文件
```
src/tests/common/event/
├── CMakeLists.txt
├── event_test.cpp
├── event_bus_test.cpp
└── handler_test.cpp
```

### event_bus_test.cpp
```cpp
#include <catch2/catch_test_macros.hpp>
#include <common/event/event_bus.h>
#include <common/event/event.h>
#include <atomic>
#include <thread>

using namespace rendu::event;

// 示例事件
class TestEvent : public TypedEvent<TestEvent> {
public:
    static std::string static_type() { return "TestEvent"; }
    int value;
};

TEST_CASE("EventBus subscribe and publish", "[event][event_bus]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    EventBus bus(io);
    std::atomic<int> count{0};

    bus.subscribe<TestEvent>([&count](const Event& e) {
        const TestEvent* te = dynamic_cast<const TestEvent*>(&e);
        if (te) count++;
    });

    TestEvent event;
    event.value = 42;
    bus.publish(event);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    REQUIRE(count == 1);

    io.stop();
}

TEST_CASE("EventBus async publish", "[event][event_bus]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    EventBus bus(io);
    std::atomic<bool> executed{false};

    bus.subscribe<TestEvent>([&executed](const Event&) {
        executed = true;
    });

    TestEvent event;
    bus.publish_async(event);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(executed == true);

    io.stop();
}
```

---

## 验收标准

### 功能
- [ ] 事件订阅/发布正确
- [ ] 支持同步和异步分发
- [ ] 订阅者可正常取消
- [ ] 多线程安全

### 性能
- [ ] 同步分发延迟 < 1µs
- [ ] 异步分发延迟 < 10ms
- [ ] 支持 10k+ 事件/秒

---

## 下一步
完成本阶段后，进入 **阶段 7: Common 层 - 配置管理 (config)**
