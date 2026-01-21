# 阶段 12: Core 层 - 生命周期 (lifecycle)

## 目标
- 定义组件生命周期接口
- 实现统一的初始化/销毁流程
- 管理组件依赖

---

## 文件结构

```
src/core/
├── include/core/lifecycle/
│   ├── lifecycle.h
│   └── lifecycle_manager.h
└── src/lifecycle/
    ├── lifecycle.cpp
    └── lifecycle_manager.cpp
```

---

## 任务清单

### 1. 生命周期接口 (lifecycle.h/cpp)
- [ ] ILifecycle 接口
- [ ] 初始化/销毁回调
- [ ] 依赖声明

### 2. 生命周期管理器 (lifecycle_manager.h/cpp)
- [ ] 组件注册
- [ ] 依赖解析
- [ ] 按序初始化/销毁

---

## 头文件设计

### lifecycle.h
```cpp
#pragma once

#include <string>
#include <vector>

namespace rendu::core {

class ILifecycle {
public:
    virtual ~ILifecycle() = default;

    virtual void initialize() = 0;
    virtual void shutdown() = 0;

    virtual std::vector<std::string> dependencies() const { return {}; }
};

} // namespace rendu::core
```

### lifecycle_manager.h
```cpp
#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <core/lifecycle/lifecycle.h>

namespace rendu::core {

class LifecycleManager {
public:
    LifecycleManager() = default;
    ~LifecycleManager();

    // 注册组件
    void register_component(std::string name, std::shared_ptr<ILifecycle> component);

    // 初始化所有组件
    void initialize_all();

    // 销毁所有组件
    void shutdown_all();

    // 获取组件
    template<typename T>
    T* get(const std::string& name) {
        auto it = components_.find(name);
        return (it != components_.end()) ? dynamic_cast<T*>(it->second.get()) : nullptr;
    }

private:
    // 拓扑排序
    std::vector<std::string> topological_sort();

    std::unordered_map<std::string, std::shared_ptr<ILifecycle>> components_;
    bool initialized_{false};
};

} // namespace rendu::core
```

---

## 单元测试

### 测试文件
```
src/tests/core/lifecycle/
├── CMakeLists.txt
├── lifecycle_test.cpp
└── lifecycle_manager_test.cpp
```

### lifecycle_manager_test.cpp
```cpp
#include <catch2/catch_test_macros.hpp>
#include <core/lifecycle/lifecycle_manager.h>
#include <atomic>

using namespace rendu::core;

class TestComponent : public ILifecycle {
public:
    explicit TestComponent(std::string name) : name_(name) {}

    void initialize() override { initialized = true; }
    void shutdown() override { shutdown_called = true; }

    std::vector<std::string> dependencies() const override {
        return deps_;
    }

    std::string name_;
    std::vector<std::string> deps_;
    std::atomic<bool> initialized{false};
    std::atomic<bool> shutdown_called{false};
};

TEST_CASE("LifecycleManager init and shutdown", "[core][lifecycle]") {
    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");
    auto comp2 = std::make_shared<TestComponent>("Comp2");

    manager.register_component("Comp1", comp1);
    manager.register_component("Comp2", comp2);

    manager.initialize_all();
    REQUIRE(comp1->initialized);
    REQUIRE(comp2->initialized);

    manager.shutdown_all();
    REQUIRE(comp1->shutdown_called);
    REQUIRE(comp2->shutdown_called);
}

TEST_CASE("LifecycleManager dependencies", "[core][lifecycle]") {
    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");
    auto comp2 = std::make_shared<TestComponent>("Comp2");
    comp2->deps_ = {"Comp1"}; // Comp2 依赖 Comp1

    manager.register_component("Comp1", comp1);
    manager.register_component("Comp2", comp2);

    manager.initialize_all();

    // 验证初始化顺序
    // Comp1 应先初始化
}
```

---

## 验收标准

### 功能
- [ ] 组件按正确顺序初始化/销毁
- [ ] 依赖解析正确
- [ ] 无资源泄漏

### 可靠性
- [ ] 循环依赖检测
- [ ] 重复初始化保护

---

## 下一步
完成本阶段后，进入 **阶段 12.5: Core 层 - Actor 系统 (actor)**
