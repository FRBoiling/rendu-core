# 阶段 13: 应用层 - 示例程序 (example)

## 目标
- 演示框架使用方法
- 验证各模块集成
- 提供开发参考

---

## 文件结构

```
src/examples/
├── CMakeLists.txt
├── README.md
├── example.h
├── example_messages.h
├── log_example.cpp
├── net_example.cpp
├── event_example.cpp
├── ecs_example.cpp
├── actor_example.cpp
├── engine_example.cpp
└── main.cpp
```

---

## 任务清单

### 1. 日志示例 (log_example.cpp)
- [x] 创建不同级别的 Logger
- [x] 演示控制台/文件 Sink
- [x] 演示格式化器

### 2. 网络示例 (net_example.cpp)
- [x] 创建 TCP 客户端/服务器
- [x] 演示自定义编解码
- [x] 演示异步通信

### 3. 事件示例 (event_example.cpp)
- [x] 自定义事件
- [x] 订阅/发布事件
- [x] 演示同步/异步分发

### 4. ECS 示例 (ecs_example.cpp)
- [x] 创建实体和组件
- [x] 演示系统执行
- [x] 演示视图查询

### 5. Actor 示例 (actor_example.cpp)
- [x] 创建 Actor
- [x] 演示 Tell/Ask 模式
- [x] 演示消息传递

### 6. 引擎示例 (engine_example.cpp)
- [x] 初始化引擎
- [x] 启动主循环
- [x] 演示模块加载

---

## 示例代码

### log_example.cpp
```cpp
#include <core/engine/engine.h>
#include <common/log/logger.h>
#include <common/log/console_sink.h>
#include <common/log/file_sink.h>
#include <common/io/io_context.h>

using namespace rendu;

int main() {
    engine::Engine engine;
    engine.initialize();

    auto& logger = engine.context().logger();
    logger.add_sink(std::make_shared<log::ConsoleSink>());

    RENDU_LOG_INFO("Engine started");
    RENDU_LOG_WARN("This is a warning");
    RENDU_LOG_ERROR("This is an error");

    engine.start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    engine.stop();

    return 0;
}
```

### actor_example.cpp
```cpp
#include <core/actor/actor_system.h>
#include <core/actor/actor.h>
#include <core/actor/message.h>
#include <common/io/io_context.h>

using namespace rendu;

class PingActor : public actor::Actor {
public:
    PingActor(std::string name, actor::ActorRef pong)
        : Actor(name), pong_(pong) {}

    void receive(const actor::Message& msg) override {
        const actor::TextMessage* tm = dynamic_cast<const actor::TextMessage*>(&msg);
        if (tm && tm->text() == "pong") {
            RENDU_LOG_INFO("Ping received pong");
            tell(pong_, std::make_shared<actor::TextMessage>("ping"));
        }
    }

private:
    actor::ActorRef pong_;
};

class PongActor : public actor::Actor {
public:
    explicit PongActor(std::string name) : Actor(name) {}

    void receive(const actor::Message& msg) override {
        const actor::TextMessage* tm = dynamic_cast<const actor::TextMessage*>(&msg);
        if (tm && tm->text() == "ping") {
            RENDU_LOG_INFO("Pong received ping");
        }
    }
};

int main() {
    io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    actor::ActorSystem system(io);
    auto pong = system.create_actor<PongActor>("pong");
    auto ping = system.create_actor<PingActor>("ping", pong);

    ping.tell(std::make_shared<actor::TextMessage>("start"));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    io.stop();

    return 0;
}
```

---

## 验收标准

### 功能
- [x] 所有示例代码已创建
- [x] CMakeLists.txt 配置完成
- [x] 文档清晰
- [x] 代码结构完整

### 可用性
- [x] 代码易于理解
- [x] 注释清晰
- [x] 可作为开发模板
- [x] 涵盖所有核心模块

---

## 下一步
完成本阶段后，进入 **阶段 14: 应用层 - 服务器 (server)**
