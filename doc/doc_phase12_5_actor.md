# 阶段 12.5: Core 层 - Actor 系统 (actor)

## 目标
- 实现轻量级 Actor 模型
- 提供高并发、消息驱动的并发编程能力
- 支持本地和跨进程通信

---

## 文件结构

```
src/core/
├── include/core/actor/
│   ├── actor.h
│   ├── actor_system.h
│   ├── actor_ref.h
│   ├── message.h
│   └── message_queue.h
└── src/actor/
    ├── actor.cpp
    ├── actor_system.cpp
    ├── actor_ref.cpp
    ├── message.cpp
    └── message_queue.cpp
```

---

## 任务清单

### 1. Actor 抽象 (actor.h/cpp)
- [ ] Actor 基类
- [ ] 消息接收接口
- [ ] 生命周期管理

### 2. Actor 系统 (actor_system.h/cpp)
- [ ] Actor 创建/销毁
- [ ] Actor 路由
- [ ] 消息分发
- [ ] 线程池管理

### 3. Actor 引用 (actor_ref.h/cpp)
- [ ] Actor 句柄
- [ ] Tell/Ask 模式
- [ ] 可序列化

### 4. 消息定义 (message.h/cpp)
- [ ] 消息基类
- [ ] 消息类型标识
- [ ] 消息序列化

### 5. 消息队列 (message_queue.h/cpp)
- [ ] 队列实现
- [ ] 优先级支持
- [ ] 限流支持

---

## 头文件设计

### actor.h
```cpp
#pragma once

#include <string>
#include <core/actor/message.h>

namespace rendu::actor {

class ActorSystem;
class ActorRef;

class Actor {
public:
    explicit Actor(std::string name);
    virtual ~Actor() = default;

    // 接收消息
    virtual void receive(const Message& message) = 0;

    // 生命周期
    virtual void on_start() {}
    virtual void on_stop() {}

    // 获取引用
    ActorRef self() const;

    const std::string& name() const;

    void set_system(ActorSystem* system);

protected:
    // 发送消息
    void tell(const ActorRef& to, std::shared_ptr<Message> message);

private:
    std::string name_;
    ActorSystem* system_;
};

} // namespace rendu::actor
```

### actor_system.h
```cpp
#pragma once

#include <memory>
#include <unordered_map>
#include <common/io/io_context.h>

namespace rendu::actor {

class Actor;
class ActorRef;
class Message;

class ActorSystem {
public:
    explicit ActorSystem(io::IoContext& io);
    ~ActorSystem();

    // 创建 Actor
    template<typename T, typename... Args>
    ActorRef create_actor(const std::string& name, Args&&... args) {
        auto actor = std::make_shared<T>(name, std::forward<Args>(args)...);
        actor->set_system(this);
        actors_[name] = actor;
        actor->on_start();
        return ActorRef(actor);
    }

    // 获取 Actor
    ActorRef get_actor(const std::string& name) const;

    // 停止 Actor
    void stop_actor(const std::string& name);

    // 发送消息
    void send(const ActorRef& to, std::shared_ptr<Message> message);

    // 路由
    io::IoContext& io();

private:
    io::IoContext& io_;
    std::unordered_map<std::string, std::shared_ptr<Actor>> actors_;
};

} // namespace rendu::actor
```

### actor_ref.h
```cpp
#pragma once

#include <string>
#include <memory>
#include <future>
#include <core/actor/message.h>

namespace rendu::actor {

class Actor;

class ActorRef {
public:
    explicit ActorRef(std::shared_ptr<Actor> actor);
    ~ActorRef() = default;

    // Tell: 发送不等待
    void tell(std::shared_ptr<Message> message);

    // Ask: 发送并等待响应
    template<typename Response>
    std::future<Response> ask(std::shared_ptr<Message> message);

    const std::string& name() const;

    // 序列化支持
    std::string serialize() const;
    static ActorRef deserialize(const std::string& data);

    bool is_valid() const;

private:
    std::shared_ptr<Actor> actor_;
    std::string path_;
};

} // namespace rendu::actor
```

### message.h
```cpp
#pragma once

#include <string>
#include <memory>

namespace rendu::actor {

class Message {
public:
    Message() = default;
    virtual ~Message() = default;

    virtual std::string type() const = 0;

    // 可选：序列化
    virtual std::string serialize() const { return ""; }
};

template<typename T>
class TypedMessage : public Message {
public:
    static std::string static_type() { return T::static_type(); }
    std::string type() const override { return T::static_type(); }
};

// 简单文本消息
class TextMessage : public TypedMessage<TextMessage> {
public:
    static std::string static_type() { return "TextMessage"; }

    explicit TextMessage(std::string text)
        : text_(std::move(text)) {}

    const std::string& text() const { return text_; }

private:
    std::string text_;
};

} // namespace rendu::actor
```

### message_queue.h
```cpp
#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <core/actor/message.h>

namespace rendu::actor {

class MessageQueue {
public:
    MessageQueue();
    ~MessageQueue() = default;

    // 入队
    void push(std::shared_ptr<Message> message, int priority = 0);

    // 出队（阻塞）
    std::shared_ptr<Message> pop();

    // 尝试出队（非阻塞）
    std::shared_ptr<Message> try_pop();

    // 大小
    size_t size() const;

private:
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    struct PriorityItem {
        int priority;
        std::shared_ptr<Message> message;
        bool operator<(const PriorityItem& other) const {
            return priority < other.priority;
        }
    };
    std::priority_queue<PriorityItem> queue_;
};

} // namespace rendu::actor
```

---

## 单元测试

### 测试文件
```
src/tests/core/actor/
├── CMakeLists.txt
├── actor_test.cpp
├── actor_system_test.cpp
├── actor_ref_test.cpp
├── message_test.cpp
└── message_queue_test.cpp
```

### actor_system_test.cpp
```cpp
#include <catch2/catch_test_macros.hpp>
#include <core/actor/actor_system.h>
#include <core/actor/actor.h>
#include <core/actor/message.h>
#include <atomic>
#include <thread>

using namespace rendu::actor;
using namespace rendu::io;

class EchoActor : public Actor {
public:
    explicit EchoActor(std::string name) : Actor(name) {}

    void receive(const Message& message) override {
        const TextMessage* tm = dynamic_cast<const TextMessage*>(&message);
        if (tm) {
            received_count++;
            last_message = tm->text();
        }
    }

    std::atomic<int> received_count{0};
    std::string last_message;
};

TEST_CASE("ActorSystem create and send", "[core][actor]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    ActorSystem system(io);
    auto echo_actor = system.create_actor<EchoActor>("echo");

    echo_actor.tell(std::make_shared<TextMessage>("hello"));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    io.stop();
}
```

---

## 验收标准

### 功能
- [ ] Actor 创建/销毁正常
- [ ] 消息传递正确（Tell/Ask）
- [ ] 多线程安全
- [ ] ActorRef 可序列化

### 性能
- [ ] 消息延迟 < 1ms（本地）
- [ ] 支持 10k+ Actor

---

## 下一步
完成本阶段后，进入 **阶段 13: 应用层 - 示例程序 (example)**
