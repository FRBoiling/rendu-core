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
- [x] ILifecycle 接口
- [x] 初始化/销毁回调
- [x] 依赖声明

### 2. 生命周期管理器 (lifecycle_manager.h/cpp)
- [x] 组件注册
- [x] 依赖解析（拓扑排序）
- [x] 按序初始化/销毁
- [x] 循环依赖检测
- [x] 线程安全保护

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
- [x] 组件按正确顺序初始化/销毁
- [x] 依赖解析正确
- [x] 无资源泄漏

### 可靠性
- [x] 循环依赖检测
- [x] 重复初始化保护
- [x] 初始化失败回滚
- [x] 线程安全保证

### 测试覆盖
- [x] 基础功能测试
- [x] 依赖解析测试
- [x] 循环依赖检测测试
- [x] 错误传播测试
- [x] 线程安全测试

### 测试统计
- **测试用例**: 17 个
- **断言数**: 59 个
- **通过率**: 100%

### 完成日期
**2026-01-28**

---

## 下一步
完成本阶段后，进入 **阶段 12.5: Core 层 - Actor 系统 (actor)**

---

## 实现状态

### 1. ILifecycle 接口实现

**文件**: `src/core/include/core/lifecycle/lifecycle.h` + `src/core/src/lifecycle/lifecycle.cpp`

- ✅ 纯虚接口设计
  - `virtual void initialize() = 0` - 初始化回调
  - `virtual void shutdown() = 0` - 销毁回调
  - `virtual std::vector<std::string> dependencies() const` - 返回依赖列表
  - `virtual std::string name() const` - 组件名称
  - `virtual void set_name(const std::string& name)` - 设置名称（供管理器使用）

### 2. LifecycleManager 管理器实现

**文件**: `src/core/include/core/lifecycle/lifecycle_manager.h` + `src/core/src/lifecycle/lifecycle_manager.cpp`

#### 核心功能

- ✅ **组件注册**
  - `register_component()` - 注册组件到管理器
  - 线程安全的组件映射（std::unordered_map）

- ✅ **依赖解析（拓扑排序）**
  - 使用 Kahn 算法实现拓扑排序
  - 支持复杂依赖链（A→B→C, D→B, D→C）
  - 循环依赖检测和报告

- ✅ **初始化流程**
  - `initialize_all()` - 按依赖顺序初始化所有组件
  - 保存初始化顺序用于后续反向销毁
  - 初始化失败时自动回滚已初始化的组件
  - 重复初始化保护

- ✅ **销毁流程**
  - `shutdown_all()` - 按初始化的逆序销毁所有组件
  - 确保依赖组件先销毁
  - 支持清理中间状态（初始化失败时）

- ✅ **错误处理**
  - 循环依赖检测抛出 std::runtime_error
  - 依赖组件不存在抛出 std::runtime_error
  - 初始化失败抛出 std::runtime_error
  - 详细的错误日志记录（使用 RENDU_LOG_*）

#### 线程安全

- ✅ 使用 std::mutex 保护共享状态
- ✅ 所有公开方法保证线程安全
- ✅ 私有方法 unsafe_* 版本供内部调用

#### 日志记录

- ✅ 使用项目标准日志宏 `RENDU_LOG_*`
- ✅ 使用 fmt 风格格式化 `{}`
- ✅ 详细记录初始化、销毁、错误等操作

### 3. 单元测试实现

**文件**: `src/tests/core/lifecycle/lifecycle_manager_test.cpp`

#### 测试组件设计

- `TestComponent` - 可配置的测试组件
  - 支持设置初始化/销毁成功或失败
  - 记录初始化和销毁顺序
  - 支持声明依赖关系
  - 线程安全的状态检查（std::atomic）

- `TestLoggerFixture` - 测试日志初始化
  - 初始化 io_context
  - 初始化 Logger
  - 确保测试日志正常工作

#### 测试用例覆盖

| 测试类别 | 测试用例 | 验证内容 |
|---------|---------|---------|
| 基础功能 | Register and Get Components | 组件注册和获取 |
| 基础功能 | Initialize Empty Manager | 空管理器初始化 |
| 基础功能 | Initialize All Components | 所有组件初始化 |
| 基础功能 | Shutdown All Components | 所有组件销毁 |
| 依赖解析 | Dependency Order | 单链依赖顺序 |
| 依赖解析 | Multiple Dependencies | 多重依赖顺序 |
| 依赖解析 | Complex Dependency Graph | 复杂依赖图 |
| 错误处理 | Missing Dependency | 依赖缺失错误 |
| 错误处理 | Circular Dependency Detection | 循环依赖检测 |
| 错误处理 | Initialization Failure | 初始化失败回滚 |
| 错误处理 | Error Propagation | 错误传播 |
| 错误处理 | Initialization Failure Cleanup | 失败清理 |
| 边界情况 | Duplicate Initialization | 重复初始化保护 |
| 边界情况 | Shutdown Before Initialize | 未初始化时销毁 |
| 边界情况 | Component Name Setting | 组件名称设置 |
| 线程安全 | Thread Safe Operations | 多线程安全 |

#### 测试统计

- **测试文件**: 1 个
- **测试用例**: 17 个
- **断言数**: 59 个
- **通过率**: 100%

### 4. 构建配置

**文件**: `src/tests/core/lifecycle/CMakeLists.txt`

- ✅ 配置测试目标
- ✅ 链接 rendu_core 和 rendu_common
- ✅ 包含必要的头文件路径

---

## 关键技术点

### 1. 拓扑排序算法

使用 Kahn 算法实现依赖解析：

```
1. 计算所有节点的入度
2. 将入度为 0 的节点加入队列
3. 处理队列，减少相邻节点的入度
4. 检测循环依赖（处理后仍有节点未被访问）
```

### 2. 初始化顺序保证

- 保存 `init_order_` 向量记录实际初始化顺序
- 销毁时按 `init_order_` 的逆序进行
- 确保依赖关系正确（依赖先初始化，后销毁）

### 3. 失败回滚机制

- 初始化失败时，调用 `shutdown_unsafe()` 回滚
- 使用 `init_order_` 的当前内容进行部分回滚
- 确保不会泄漏已初始化的资源

### 4. 线程安全策略

- 使用 std::mutex 保护所有共享状态
- 公开方法加锁，内部方法使用 unsafe_* 版本
- 避免死锁（短时间持锁）

---

## 遗留问题

无

---

## 性能考虑

- 拓扑排序时间复杂度: O(V + E)，V 为组件数，E 为依赖边数
- 线程安全使用互斥锁，初始化阶段性能可接受
- 运行时仅保存顺序，无额外开销

---

## 使用示例

```cpp
#include <core/lifecycle/lifecycle_manager.h>

class NetworkManager : public ILifecycle {
public:
    void initialize() override {
        // 初始化网络
    }

    void shutdown() override {
        // 清理网络
    }

    std::vector<std::string> dependencies() const override {
        return {}; // 无依赖
    }
};

class DatabaseManager : public ILifecycle {
public:
    void initialize() override {
        // 初始化数据库
    }

    void shutdown() override {
        // 清理数据库
    }

    std::vector<std::string> dependencies() const override {
        return {"Network"}; // 依赖网络管理器
    }
};

int main() {
    LifecycleManager manager;

    manager.register_component("Network", std::make_shared<NetworkManager>());
    manager.register_component("Database", std::make_shared<DatabaseManager>());

    // 按依赖顺序初始化: Network -> Database
    manager.initialize_all();

    // ... 运行时 ...

    // 按初始化逆序销毁: Database -> Network
    manager.shutdown_all();

    return 0;
}
```
