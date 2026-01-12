# ECS 框架并行优化指南

## 优化概述

本次优化主要针对 **CPU 利用率最大化**，实现了以下核心功能：

### 1. 依赖感知的并行执行
- 系统按依赖关系分层，同层系统可并行执行
- 支持串行/并行系统混合调度
- 确保依赖系统按正确顺序执行

### 2. 数据并行化支持
- `ParallelFor` 工具类支持范围并行遍历
- `SystemBase` 提供便捷的 `parallel_each()` 和 `parallel_for_range()` 方法
- 自动任务分块，可配置 chunk_size

### 3. 灵活的执行模式
- `SystemExecutionMode::PARALLEL` - 可并行执行（默认）
- `SystemExecutionMode::SEQUENTIAL` - 必须串行执行

---

## 使用方法

### 基本用法

```cpp
#include "common/ecs/world.h"
#include "core/ecs/example_systems.h"

class MyApplication : public Ecs::Application
{
protected:
    void register_systems() override
    {
        // 添加系统，框架自动处理依赖排序
        world_->add_system<MovementSystem>();
        world_->add_system<ParallelMovementSystem>();
        world_->add_system<HealthRegenSystem>();
        world_->add_system<RenderSystem>();
    }

    void update(float delta_time) override
    {
        // world_->update_systems(delta_time) 已在 run() 中自动调用
    }
};
```

**注意:** 使用 `Ecs::System<MySystem>` 而非 `Ecs::SystemBase<MySystem>` 来定义系统。

### 定义新系统

```cpp
#include "common/ecs/world.h"

class MySystem : public Ecs::System<MySystem>
{
public:
    std::string get_name() const override { return "MySystem"; }

    // 可选：指定依赖
    std::vector<std::string> get_dependencies() const override
    {
        return {"SomeOtherSystem"};
    }

    // 可选：指定执行模式
    SystemExecutionMode get_execution_mode() const override
    {
        return SystemExecutionMode::PARALLEL;  // 或 SEQUENTIAL
    }

    void update(float delta_time) override
    {
        // 方式1: 单线程遍历
        each<TransformComponent>([](Entity e, TransformComponent* transform) {
            // 处理实体
        });

        // 方式2: 自动并行遍历（默认 chunk_size=256）
        parallel_each<TransformComponent>([](Entity e, TransformComponent* transform) {
            // 处理实体（并行）
        });

        // 方式3: 自定义 chunk_size
        parallel_each<TransformComponent>([](Entity e, TransformComponent* transform) {
            // 处理实体（并行）
        }, 512);  // 每个任务处理512个实体

        // 方式4: 手动控制分块
        parallel_for_range<TransformComponent>(
            [](Entity e, size_t index, TransformComponent* transform) {
                // 可以使用 index 做额外处理
            },
            1024  // chunk_size
        );
    }
};
```

---

## 性能优化建议

### 1. 系统依赖设计

```cpp
// ❌ 不好的设计：所有系统都无依赖，可能破坏执行顺序
class SystemA : public System<SystemA> {
    std::vector<std::string> get_dependencies() const override { return {}; }
};
class SystemB : public System<SystemB> {
    std::vector<std::string> get_dependencies() const override { return {}; }
};

// ✅ 好的设计：明确依赖关系
class SystemA : public System<SystemA> {
    std::vector<std::string> get_dependencies() const override { return {}; }
};
class SystemB : public System<SystemB> {
    std::vector<std::string> get_dependencies() const override { return {"SystemA"}; }
};
```

### 2. 选择合适的执行模式

| 场景 | 执行模式 | 说明 |
|------|----------|------|
| 纯计算（如物理模拟、路径查找） | PARALLEL | 可充分利用多核 |
| 状态共享（如全局计数器、单例） | SEQUENTIAL | 避免竞争条件 |
| GPU 提交（如渲染命令） | SEQUENTIAL | API 通常要求串行 |

### 3. Chunk Size 选择

```cpp
// 小实体（简单数据）：更大的 chunk
parallel_each<TransformComponent>(func, 512);

// 大实体（复杂处理）：更小的 chunk
parallel_each<PhysicsComponent>(func, 64);

// 需要平衡：避免任务过多导致开销
parallel_each<Component>(func, 256);  // 默认值通常适用
```

### 4. 避免过细粒度的并行

```cpp
// ❌ 避免为每个实体创建一个任务
for (auto e : entities) {
    thread_pool.Submit([e]() { process(e); });  // 任务过多
}

// ✅ 使用分块处理
ParallelFor::each(entities, [](Entity e) {
    process(e);
}, thread_pool, 256);  // 每256个实体一个任务
```

---

## 并行执行示例

假设有4个系统，依赖关系如下：

```
SystemA ───┐
           ├──> SystemC ───> SystemD
SystemB ───┘
```

执行层级和并行情况：

| 层级 | 系统 | 并行数 |
|------|------|--------|
| Level 0 | SystemA, SystemB | 2 (可并行) |
| Level 1 | SystemC | 1 |
| Level 2 | SystemD | 1 |

如果 `SystemA` 和 `SystemB` 都标记为 `PARALLEL`，它们将在第一层同时运行。

---

## 调试建议

### 查看系统分组

框架启动时会输出系统分层信息：

```
[INFO] application: Systems grouped into 3 parallel levels
[INFO] application:   Level 0: 2 systems
[INFO] application:   Level 1: 1 systems
[INFO] application:   Level 2: 1 systems
```

### 使用日志追踪

```cpp
void update(float delta_time) override
{
    RC_LOG_DEBUG("system", "{}: update called", get_name());
    // ...
}
```

---

## 技术细节

### 线程池类型

当前使用 `WorkStealingThreadPoolType::Basic`，可在 `world.cpp` 中修改为 `Advanced`：

```cpp
thread_pool_(std::make_unique<Threading::ThreadPoolAdapter>(
    Threading::WorkStealingThreadPoolType::Advanced,  // 或 Basic
    std::thread::hardware_concurrency()
))
```

### 文件结构

| 文件 | 说明 |
|------|------|
| `src/common/include/common/ecs/parallel_for.h` | 并行遍历工具 |
| `src/common/include/common/ecs/i_system.h` | ISystem 基类 + System 模板 |
| `src/common/include/common/ecs/world.h` | World 类 |
| `src/core/include/core/ecs/example_systems.h` | 示例系统 |
| `ECS_OPTIMIZATION_GUIDE.md` | 使用指南 |

### API 变更

```cpp
// 旧版本 (已废弃)
#include "common/ecs/system_base.h"
class MySystem : public Ecs::SystemBase<MySystem> { ... };

// 新版本
#include "common/ecs/world.h"  // 或 #include "common/ecs/i_system.h"
class MySystem : public Ecs::System<MySystem> { ... };
```

### 依赖图算法

使用 **Kahn 拓扑排序算法** 计算系统层级：
- O(V + E) 时间复杂度
- V = 系统数量
- E = 依赖边数量

### 并行执行保证

- 同层系统：可并行（除标记为 SEQUENTIAL 的系统）
- 跨层系统：严格按依赖顺序执行
- 层内顺序：先执行所有 PARALLEL 系统，再执行 SEQUENTIAL 系统
