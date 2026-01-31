# RenduCore 用户指南

**版本**: 0.2.0
**更新日期**: 2026-02-01

---

## 目录

1. [快速入门](#快速入门)
2. [核心概念](#核心概念)
3. [基础模块使用](#基础模块使用)
4. [高级特性](#高级特性)
5. [最佳实践](#最佳实践)
6. [常见问题](#常见问题)

---

## 快速入门

### 1.1 环境准备

#### 前置要求
- CMake >= 3.24
- C++20 兼容编译器
  - GCC 9+
  - Clang 12+
  - MSVC 19+

#### 安装依赖

**macOS**:
```bash
brew install cmake boost protobuf
```

**Ubuntu/Debian**:
```bash
sudo apt install cmake libboost-all-dev libprotobuf-dev
```

**Windows**:
- 使用 vcpkg 安装依赖：
```bash
vcpkg install boost-asio boost-system protobuf cmake
```

### 1.2 构建项目

```bash
# 克隆项目
git clone https://github.com/xxx/rendu-core.git
cd rendu-core

# 配置构建（Debug 模式）
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug

# 编译项目
cmake --build cmake-build-debug

# 或者使用 Release 模式
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release
```

### 1.3 运行示例

```bash
# 运行日志示例
./cmake-build-debug/src/examples/log_example

# 运行网络示例
./cmake-build-debug/src/examples/net_example

# 运行事件示例
./cmake-build-debug/src/examples/event_example

# 运行配置示例
./cmake-build-debug/src/examples/config_example
```

### 1.4 运行测试

```bash
# 运行所有测试
ctest --test-dir cmake-build-debug

# 运行特定测试套件
./cmake-build-debug/src/tests/common/log/logger_test

# 查看详细输出
ctest --test-dir cmake-build-debug --verbose
```

### 1.5 第一个程序

创建一个简单的日志程序：

```cpp
#include "common/log/logger.h"
#include "common/log/console_sink.h"
#include "common/io/io_context.h"

using namespace Rendu;

int main() {
    // 创建 IoContext
    io::IoContext io(1);

    // 初始化默认日志系统
    log::init_default_io_context(io);
    auto& logger = log::default_logger();
    logger.add_sink(std::make_shared<log::ConsoleSink>());

    // 在独立线程运行 io_context
    std::thread([&io]() { io.run(); }).detach();

    // 记录日志
    logger.info("Hello, RenduCore!");
    logger.warn("This is a warning message");
    logger.error("This is an error message");

    // 等待日志输出
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    io.stop();
    return 0;
}
```

**编译并运行**:

```bash
g++ -std=c++20 -I/path/to/rendu-core/include \
    main.cpp -o my_program \
    -L/path/to/rendu-core/lib \
    -lRenduCore-common -lboost_system -lpthread

./my_program
```

---

## 核心概念

### 2.1 架构概览

RenduCore 采用分层架构设计：

```
┌─────────────────────────────────────────┐
│         应用层 (Apps)                  │
│  - 服务器 (server)                    │
│  - 客户端 (client)                    │
└─────────────────────────────────────────┘
              ↓
┌─────────────────────────────────────────┐
│         核心层 (Core)                  │
│  - 引擎核心 (engine)                  │
│  - 主循环 (loop)                      │
│  - 状态管理 (state)                   │
│  - 生命周期 (lifecycle)               │
│  - Actor 系统 (actor)                 │
└─────────────────────────────────────────┘
              ↓
┌─────────────────────────────────────────┐
│         公共层 (Common)                │
│  - 基础工具 (util)                  │
│  - I/O 抽象 (io)                     │
│  - 日志系统 (log)                     │
│  - 网络通信 (net)                    │
│  - 序列化 (ser)                       │
│  - 事件系统 (event)                   │
│  - 配置管理 (config)                  │
│  - ECS 系统 (ecs)                     │
└─────────────────────────────────────────┘
```

### 2.2 核心组件

#### IoContext (I/O 上下文)
- 基于 boost::asio 的事件循环
- 支持多线程并发
- 提供异步任务调度

#### Logger (日志系统)
- 支持多级别日志 (TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL)
- 支持多种输出目标 (控制台、文件)
- 异步日志记录，不阻塞主线程

#### EventBus (事件总线)
- 发布-订阅模式
- 支持同步和异步事件处理
- 类型安全的事件传递

#### Config (配置管理)
- 支持多种格式 (JSON, Protobuf)
- 支持嵌套配置
- 支持配置热更新

#### Actor (Actor 模型)
- 消息驱动的并发模型
- 隔离的执行上下文
- 适合构建高并发服务

---

## 基础模块使用

### 3.1 日志系统

#### 基本使用

```cpp
#include "common/log/logger.h"
#include "common/log/console_sink.h"
#include "common/io/io_context.h"

using namespace Rendu;

int main() {
    io::IoContext io(1);
    log::init_default_io_context(io);

    auto& logger = log::default_logger();
    logger.add_sink(std::make_shared<log::ConsoleSink>());

    std::thread([&io]() { io.run(); }).detach();

    // 记录不同级别的日志
    logger.trace("详细追踪信息");
    logger.debug("调试信息");
    logger.info("普通信息");
    logger.warn("警告信息");
    logger.error("错误信息");
    logger.critical("严重错误");

    // 格式化日志（使用 fmt 格式）
    logger.info("Server started on port {}", 8080);
    logger.error("Failed to connect to {}:{}", host, port);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    io.stop();

    return 0;
}
```

#### 使用日志宏

```cpp
#include "common/log/logger.h"

// 在初始化后使用日志宏
RENDU_LOG_INFO("Hello, World!");
RENDU_LOG_WARN("Warning: {}", warning_msg);
RENDU_LOG_ERROR("Error occurred: {}", error_code);
```

#### 文件日志

```cpp
#include "common/log/file_sink.h"

auto file_sink = std::make_shared<log::FileSink>("app.log");
logger.add_sink(file_sink);
```

### 3.2 网络通信

#### TCP 客户端

```cpp
#include "common/net/socket.h"
#include "common/net/channel.h"
#include "common/io/io_context.h"

using namespace Rendu;

int main() {
    io::IoContext io(1);

    // 创建 TCP Socket
    net::Socket socket(io);

    // 连接到服务器
    socket.connect("127.0.0.1", 8080);

    // 创建通道
    net::Channel channel(socket);

    // 发送数据
    channel.send("Hello, Server!");

    // 接收数据
    auto msg = channel.receive();
    if (msg) {
        std::cout << "Received: " << *msg << std::endl;
    }

    std::thread([&io]() { io.run(); }).join();

    return 0;
}
```

#### TCP 服务器

```cpp
#include "common/net/socket.h"
#include "common/io/io_context.h"

using namespace Rendu;

int main() {
    io::IoContext io(4);

    // 创建监听 Socket
    net::Socket server(io);
    server.bind("0.0.0.0", 8080);
    server.listen(100);

    std::thread([&io]() { io.run(); }).detach();

    // 接受连接
    while (true) {
        auto client = server.accept();
        std::cout << "Client connected" << std::endl;

        // 处理客户端
        net::Channel channel(*client);
        auto msg = channel.receive();
        if (msg) {
            channel.send("Echo: " + *msg);
        }
    }

    io.stop();
    return 0;
}
```

### 3.3 事件系统

#### 发布事件

```cpp
#include "common/event/event.h"
#include "common/event/event_bus.h"

// 定义自定义事件
struct PlayerEvent : public event::Event {
    int player_id;
    std::string action;

    PlayerEvent(int id, const std::string& act)
        : player_id(id), action(act) {}
};

using namespace Rendu;

int main() {
    io::IoContext io(1);
    event::EventBus bus(io);

    // 订阅事件
    bus.subscribe<PlayerEvent>([](const PlayerEvent& e) {
        std::cout << "Player " << e.player_id
                  << " performed: " << e.action << std::endl;
    });

    std::thread([&io]() { io.run(); }).detach();

    // 发布事件
    bus.publish(PlayerEvent(1, "jump"));
    bus.publish(PlayerEvent(2, "run"));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    io.stop();

    return 0;
}
```

### 3.4 配置管理

#### 加载 JSON 配置

```cpp
#include "common/config/config.h"
#include "common/config/loader.h"

using namespace Rendu::config;

int main() {
    // 加载配置
    JsonLoader loader;
    auto result = loader.load("config.json");

    if (std::holds_alternative<Error>(result)) {
        std::cerr << "Failed to load config: "
                  << std::get<Error>(result).message() << std::endl;
        return 1;
    }

    Config config = std::get<Config>(result);

    // 读取配置
    auto port_result = config.get<int64_t>("server.port");
    if (std::holds_alternative<int64_t>(port_result)) {
        int port = std::get<int64_t>(port_result);
        std::cout << "Server port: " << port << std::endl;
    }

    // 使用默认值
    int timeout = config.get_or_default<int64_t>("server.timeout", 30);

    // 访问嵌套配置
    auto db_host = config.get<std::string>("database.host");
    auto db_port = config.get<int64_t>("database.port");

    return 0;
}
```

#### 配置文件示例 (config.json)

```json
{
    "server": {
        "port": 8080,
        "host": "0.0.0.0",
        "timeout": 30
    },
    "database": {
        "host": "localhost",
        "port": 5432,
        "name": "mydb"
    }
}
```

#### 配置热更新

```cpp
#include "common/config/watcher.h"
#include "common/io/io_context.h"

using namespace Rendu;
using namespace Rendu::config;

int main() {
    io::IoContext io(2);

    // 创建配置监控器
    ConfigWatcher watcher(io, "config.json", std::chrono::seconds(1));

    // 注册变更回调
    watcher.on_change([](const Config& config) {
        std::cout << "Config reloaded!" << std::endl;
        // 更新应用配置
    });

    std::thread([&io]() { io.run(); }).detach();

    // 启动监控
    watcher.start();

    // 主循环...
    while (true) {
        // 使用当前配置
        auto config = watcher.config();
        // ...
    }

    watcher.stop();
    io.stop();

    return 0;
}
```

---

## 高级特性

### 4.1 Actor 模型

#### 创建 Actor

```cpp
#include "core/actor/actor.h"
#include "core/engine/context.h"

using namespace Rendu;

// 定义自定义 Actor
class PlayerActor : public core::Actor {
public:
    void on_receive(const Message& msg) override {
        std::cout << "Player received message: " << msg.content << std::endl;
    }
};

int main() {
    core::Context ctx;
    auto& actor_mgr = ctx.actor_manager();

    // 创建 Actor
    auto player = actor_mgr.create_actor<PlayerActor>();

    // 发送消息
    player->send("Hello, Player!");

    return 0;
}
```

### 4.2 状态机

#### 简单状态机

```cpp
#include "core/state/state.h"
#include "core/state/state_machine.h"

using namespace Rendu::core;

// 定义状态
class MenuState : public State {
public:
    void enter() override {
        std::cout << "Entering Menu" << std::endl;
    }

    void update(float dt) override {
        std::cout << "Menu update" << std::endl;
    }

    void exit() override {
        std::cout << "Exiting Menu" << std::endl;
    }
};

class GameState : public State {
public:
    void enter() override {
        std::cout << "Entering Game" << std::endl;
    }

    void update(float dt) override {
        std::cout << "Game update" << std::endl;
    }

    void exit() override {
        std::cout << "Exiting Game" << std::endl;
    }
};

int main() {
    StateMachine sm;

    // 添加状态
    sm.add_state<MenuState>("menu");
    sm.add_state<GameState>("game");

    // 切换状态
    sm.change_state("menu");
    sm.update(0.016f);
    sm.change_state("game");
    sm.update(0.016f);

    return 0;
}
```

### 4.3 ECS 系统

#### 使用 ECS

```cpp
#include "common/ecs/ecs.h"

using namespace Rendu;

// 定义组件
struct Position {
    float x, y, z;
};

struct Velocity {
    float vx, vy, vz;
};

// 定义系统
class MovementSystem {
public:
    void update(entt::registry& registry, float dt) {
        auto view = registry.view<Position, Velocity>();

        for (auto entity : view) {
            auto& pos = view.get<Position>(entity);
            auto& vel = view.get<Velocity>(entity);

            pos.x += vel.vx * dt;
            pos.y += vel.vy * dt;
            pos.z += vel.vz * dt;
        }
    }
};

int main() {
    entt::registry registry;
    MovementSystem movement;

    // 创建实体并添加组件
    auto entity = registry.create();
    registry.emplace<Position>(entity, 0.0f, 0.0f, 0.0f);
    registry.emplace<Velocity>(entity, 1.0f, 0.0f, 0.0f);

    // 更新系统
    movement.update(registry, 0.016f);

    return 0;
}
```

---

## 最佳实践

### 5.1 错误处理

**推荐做法**:
```cpp
// 使用 Result 类型处理错误
auto result = some_function();
if (std::holds_alternative<Error>(result)) {
    auto error = std::get<Error>(result);
    logger.error("Operation failed: {}", error.message());
    return;
}
auto value = std::get<ReturnType>(result);
```

**避免做法**:
```cpp
// 不要忽略错误
some_function(); // ❌ 忽略返回值

// 不要使用异常控制流
try {
    // 正常逻辑
} catch (...) {
    // ❌ 异常不应用于控制流
}
```

### 5.2 资源管理

**推荐做法**:
```cpp
// 使用 RAII 管理资源
{
    std::unique_ptr<Resource> res = create_resource();
    // 资源自动释放
}

// 使用智能指针
auto entity = std::make_shared<Entity>();
```

### 5.3 并发编程

**推荐做法**:
```cpp
// 使用 Actor 隔离线程
auto actor = actor_mgr.create_actor<MyActor>();
actor->send(message);

// 使用事件总线解耦
event_bus.publish<MyEvent>(data);
```

**避免做法**:
```cpp
// 不要直接使用互斥锁（除非必要）
std::mutex mtx;
mtx.lock();
// ❌ 可能死锁
mtx.unlock();
```

### 5.4 日志记录

**推荐做法**:
```cpp
// 记录关键操作
logger.info("User {} logged in", user_id);

// 记录错误
logger.error("Failed to process request: {}", error_msg);

// 记录调试信息
logger.debug("Processing item: {}", item_id);
```

---

## 常见问题

### Q1: 如何处理连接失败？

**A**: 检查返回的 Error 类型：

```cpp
auto result = socket.connect(host, port);
if (std::holds_alternative<Error>(result)) {
    auto error = std::get<Error>(result);
    if (error.code() == ErrorCode::ConnectionRefused) {
        // 处理连接被拒绝
    }
}
```

### Q2: 如何调试日志问题？

**A**: 检查以下几点：
1. 确认 `init_default_io_context()` 已调用
2. 确认 IoContext 正在运行
3. 确认已添加至少一个 Sink
4. 检查日志级别设置

### Q3: 如何提高性能？

**A**:
1. 使用 Release 构建模式
2. 减少日志输出（调整日志级别）
3. 使用对象池减少内存分配
4. 避免不必要的拷贝（使用移动语义）

### Q4: 如何处理高并发？

**A**:
1. 使用多线程 IoContext
2. 使用 Actor 模型隔离逻辑
3. 使用异步 I/O 避免阻塞
4. 合理设置线程池大小

### Q5: 如何测试代码？

**A**: 参考现有测试文件：

```cpp
#include <catch2/catch_test_macros.hpp>

TEST_CASE("My test case", "[module]") {
    // 准备测试数据
    int value = 42;

    // 执行测试
    SECTION("test scenario") {
        REQUIRE(value == 42);
    }
}
```

---

## 参考资料

- [设计文档索引](DOC_INDEX.md)
- [Roadmap](doc_roadmap.md)
- [API 文档](docs/api/index.html) (待生成)

---

**文档版本**: 1.0
**最后更新**: 2026-02-01
