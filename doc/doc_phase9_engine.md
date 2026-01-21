# 阶段 9: Core 层 - 引擎核心 (engine)

## 目标
- 实现主引擎类
- 管理引擎生命周期
- 提供全局上下文

---

## 文件结构

```
src/core/
├── include/core/engine/
│   ├── engine.h
│   └── context.h
└── src/engine/
    ├── engine.cpp
    └── context.cpp
```

---

## 任务清单

### 1. 引擎主类 (engine.h/cpp)
- [ ] 引擎初始化
- [ ] 启动/停止
- [ ] 生命周期管理
- [ ] 模块管理

### 2. 引擎上下文 (context.h/cpp)
- [ ] 全局状态管理
- [ ] 配置访问
- [ ] 日志访问
- [ ] 事件总线访问

---

## 头文件设计

### engine.h
```cpp
#pragma once

#include <memory>
#include <vector>
#include <common/io/io_context.h>
#include <common/log/logger.h>
#include <core/engine/context.h>

namespace rendu::core {

class Engine {
public:
    Engine();
    ~Engine();

    // 禁止拷贝和移动
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    // 初始化
    void initialize();

    // 启动/停止
    void start();
    void stop();

    // 运行状态
    bool is_running() const;

    // 访问上下文
    Context& context();

    // 模块管理
    template<typename Module, typename... Args>
    void add_module(Args&&... args) {
        modules_.push_back(std::make_shared<Module>(std::forward<Args>(args)...));
    }

private:
    void update_modules(float delta_time);

    std::unique_ptr<Context> context_;
    std::unique_ptr<io::IoContext> io_;
    std::shared_ptr<log::Logger> logger_;
    std::vector<std::shared_ptr<void>> modules_; // 简化实现
    bool running_;
};

} // namespace rendu::core
```

### context.h
```cpp
#pragma once

#include <memory>
#include <common/io/io_context.h>
#include <common/log/logger.h>
#include <common/event/event_bus.h>
#include <common/config/config.h>

namespace rendu::core {

class Context {
public:
    Context();
    ~Context();

    // 访问子系统
    io::IoContext& io();
    log::Logger& logger();
    event::EventBus& event_bus();
    config::Config& config();

    // 配置
    void load_config(const std::string& filepath);

private:
    std::unique_ptr<io::IoContext> io_;
    std::shared_ptr<log::Logger> logger_;
    std::unique_ptr<event::EventBus> event_bus_;
    config::Config config_;
};

} // namespace rendu::core
```

---

## 单元测试

### 测试文件
```
src/tests/core/engine/
├── CMakeLists.txt
├── engine_test.cpp
└── context_test.cpp
```

### engine_test.cpp
```cpp
#include <catch2/catch_test_macros.hpp>
#include <core/engine/engine.h>

using namespace rendu::core;

TEST_CASE("Engine initialize and start", "[core][engine]") {
    Engine engine;

    REQUIRE_NOTHROW(engine.initialize());
    REQUIRE_FALSE(engine.is_running());

    engine.start();
    REQUIRE(engine.is_running());

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.stop();
    REQUIRE_FALSE(engine.is_running());
}
```

---

## 验收标准

### 功能
- [ ] 引擎可正常启动/停止
- [ ] 资源正确释放
- [ ] 模块加载正确

### 可靠性
- [ ] 无内存泄漏
- [ ] 异常安全

---

## 下一步
完成本阶段后，进入 **阶段 10: Core 层 - 主循环 (loop)**
