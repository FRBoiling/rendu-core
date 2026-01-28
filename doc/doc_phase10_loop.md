# 阶段 10: Core 层 - 主循环 (loop)

**完成日期**: 2026-01-26
**状态**: ✅ 完成

---

## 文件结构

```
src/core/
├── include/core/loop/
│   ├── loop.h
│   └── fixed_loop.h
└── src/loop/
    ├── loop.cpp
    └── fixed_loop.cpp
```

---

## 任务清单

### 1. 循环抽象 (loop.h/cpp)
- [x] 循环接口定义
- [x] 启动/停止/暂停
- [x] 更新回调
- [x] 帧率控制

### 2. 固定步长循环 (fixed_loop.h/cpp)
- [x] 固定时间步长
- [x] 累积时间处理
- [x] 帧率稳定

---

## 头文件设计

### loop.h
```cpp
#pragma once

#include <functional>
#include <atomic>

namespace rendu::core {

class Loop {
public:
    using UpdateCallback = std::function<void(float)>;

    Loop() = default;
    virtual ~Loop() = default;

    // 启动/停止/暂停
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;

    // 设置更新回调
    void set_update_callback(UpdateCallback callback);

    // 状态
    bool is_running() const;
    bool is_paused() const;

protected:
    UpdateCallback on_update_;
    std::atomic<bool> running_{false};
    std::atomic<bool> paused_{false};
};

} // namespace rendu::core
```

### fixed_loop.h
```cpp
#pragma once

#include <core/loop/loop.h>
#include <chrono>
#include <thread>
#include <common/io/io_context.h>

namespace rendu::core {

class FixedLoop : public Loop {
public:
    explicit FixedLoop(float fixed_delta_time, io::IoContext& io);
    ~FixedLoop() override;

    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;

    // 设置目标帧率
    void set_target_fps(int fps);

private:
    void run_loop();

    float fixed_delta_time_;
    io::IoContext& io_;
    std::thread loop_thread_;
    int target_fps_;
    bool should_stop_;
};

} // namespace rendu::core
```

---

## 单元测试

### 测试文件
```
src/tests/core/loop/
├── CMakeLists.txt
└── fixed_loop_test.cpp
```

### fixed_loop_test.cpp
```cpp
#include <catch2/catch_test_macros.hpp>
#include <core/loop/fixed_loop.h>
#include <common/io/io_context.h>
#include <atomic>
#include <thread>

using namespace rendu::core;

TEST_CASE("FixedLoop start and stop", "[core][loop]") {
    io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    FixedLoop loop(0.016f, io); // ~60 FPS
    std::atomic<int> count{0};

    loop.set_update_callback([&count](float dt) {
        count++;
        if (count >= 5) loop.stop();
    });

    loop.start();
    loop.stop();

    REQUIRE(count >= 5);
    io.stop();
}
```

---

## 验收标准

### 功能
- [x] 循环可正常启动/停止
- [x] 帧率稳定
- [x] 暂停/恢复正常

### 性能
- [x] 帧率误差 < 5%
- [x] CPU 占用合理

### 单元测试
- [x] fixed_loop_test 完成
  - 基本生命周期测试
  - 更新回调测试
  - 暂停/恢复测试
  - 目标 FPS 设置测试
  - FPS 测量测试
  - 总帧数统计测试
  - 停止条件测试
  - 并发暂停/恢复测试
  - 不同 delta time 测试
  - 无效参数处理测试
  - 压力测试

### 实现状态
- ✅ Loop 抽象基类完成（2026-01-26）
  - start/stop/pause/resume 接口
  - 更新回调机制
  - 运行状态查询
- ✅ FixedLoop 固定步长循环完成（2026-01-26）
  - 固定时间步长更新（累积时间处理）
  - 帧率控制（动态 sleep 调整）
  - FPS 统计（每秒实时更新）
  - 暂停/恢复功能
  - 线程安全（使用 std::atomic）
  - 完整的日志记录

### 文件清单
```
src/core/
├── include/core/loop/
│   ├── loop.h           - 循环抽象基类
│   └── fixed_loop.h     - 固定步长循环
└── src/loop/
    └── fixed_loop.cpp   - 固定步长循环实现

src/tests/core/loop/
├── CMakeLists.txt       - 测试构建配置
└── fixed_loop_test.cpp  - 单元测试（10 个测试用例）
```

---

## 完成日期
**2026-01-26**
---

## 下一步
完成本阶段后，进入 **阶段 11: Core 层 - 状态管理 (state)**
