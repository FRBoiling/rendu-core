# 阶段 11: Core 层 - 状态管理 (state)

## 目标
- 实现状态机
- 管理运行时状态
- 支持状态转换和回调

---

## 文件结构

```
src/core/
├── include/core/state/
│   ├── state.h
│   └── state_machine.h
└── src/state/
    ├── state.cpp
    └── state_machine.cpp
```

---

## 任务清单

### 1. 状态基类 (state.h/cpp)
- [ ] 进入/退出回调
- [ ] 更新回调
- [ ] 状态标识

### 2. 状态机 (state_machine.h/cpp)
- [ ] 状态注册
- [ ] 状态转换
- [ ] 当前状态查询
- [ ] 转换条件判断

---

## 头文件设计

### state.h
```cpp
#pragma once

#include <string>

namespace rendu::core {

class State {
public:
    explicit State(std::string name);
    virtual ~State() = default;

    // 进入状态
    virtual void on_enter() {}

    // 离开状态
    virtual void on_exit() {}

    // 更新
    virtual void on_update(float delta_time) {}

    const std::string& name() const;

private:
    std::string name_;
};

} // namespace rendu::core
```

### state_machine.h
```cpp
#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include <core/state/state.h>

namespace rendu::core {

using TransitionCondition = std::function<bool()>;

class StateMachine {
public:
    StateMachine() = default;
    ~StateMachine() = default;

    // 注册状态
    void register_state(std::shared_ptr<State> state);

    // 设置初始状态
    void set_initial_state(const std::string& name);

    // 添加状态转换
    void add_transition(
        const std::string& from,
        const std::string& to,
        TransitionCondition condition = nullptr
    );

    // 启动
    void start();

    // 手动切换状态
    void change_state(const std::string& name);

    // 更新
    void update(float delta_time);

    // 查询
    State* current_state() const;
    bool has_state(const std::string& name) const;

private:
    bool can_transition(const std::string& to) const;

    std::unordered_map<std::string, std::shared_ptr<State>> states_;
    std::unordered_map<std::string, std::vector<std::pair<std::string, TransitionCondition>>> transitions_;
    std::string initial_state_;
    State* current_state_{nullptr};
    bool started_{false};
};

} // namespace rendu::core
```

---

## 单元测试

### 测试文件
```
src/tests/core/state/
├── CMakeLists.txt
├── state_test.cpp
└── state_machine_test.cpp
```

### state_machine_test.cpp
```cpp
#include <catch2/catch_test_macros.hpp>
#include <core/state/state_machine.h>
#include <atomic>

using namespace rendu::core;

class TestState : public State {
public:
    explicit TestState(std::string name) : State(name) {}
    std::atomic<bool> entered{false};
    std::atomic<bool> exited{false};

    void on_enter() override { entered = true; }
    void on_exit() override { exited = true; }
};

TEST_CASE("StateMachine basic", "[core][state]") {
    StateMachine sm;
    auto s1 = std::make_shared<TestState>("Idle");
    auto s2 = std::make_shared<TestState>("Running");

    sm.register_state(s1);
    sm.register_state(s2);
    sm.set_initial_state("Idle");

    sm.start();
    sm.update(0.016f);

    REQUIRE(sm.current_state()->name() == "Idle");
    REQUIRE(s1->entered == true);
}

TEST_CASE("StateMachine transition", "[core][state]") {
    StateMachine sm;
    auto s1 = std::make_shared<TestState>("Idle");
    auto s2 = std::make_shared<TestState>("Running");

    sm.register_state(s1);
    sm.register_state(s2);
    sm.set_initial_state("Idle");
    sm.add_transition("Idle", "Running", []() { return true; });

    sm.start();
    sm.update(0.016f);
    sm.update(0.016f); // 触发转换

    REQUIRE(sm.current_state()->name() == "Running");
}
```

---

## 验收标准

### 功能
- [ ] 状态转换正确
- [ ] 进入/退出回调正确执行
- [ ] 条件转换正常工作

### 可靠性
- [ ] 无状态泄漏
- [ ] 无循环转换

---

## 下一步
完成本阶段后，进入 **阶段 12: Core 层 - 生命周期 (lifecycle)**
