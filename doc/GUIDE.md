# RenduCore ECS 使用指南

## 📖 快速开始

### 创建 ECS 注册中心

```cpp
#include "common/ecs/registry_optimized.h"

using namespace Rendu;

// 创建注册中心
RegistryOptimized registry;
```

### 创建实体和组件

```cpp
// 定义组件
struct Position {
    float x, y, z;
};

struct Velocity {
    float dx, dy, dz;
};

// 创建实体并添加组件
Entity player = registry.create();
registry.emplace<Position, Velocity>(
    player,
    Position{0.0f, 0.0f, 0.0f},
    Velocity{1.0f, 0.5f, 0.0f}
);

// 批量创建实体（推荐）
auto enemies = registry.createBatch<Position, Velocity>(100);
for (auto& enemy : enemies) {
    auto [pos, vel] = registry.tryGet<Position, Velocity>(enemy);
    if (pos && vel) {
        pos->x = 10.0f;
        vel->dx = 2.0f;
    }
}
```

### 查询和遍历实体

```cpp
// 基本查询 - 所有具有 Position 和 Velocity 的实体
auto view = registry.view<Position, Velocity>();
view.each([](Entity e, Position& pos, Velocity& vel) {
    pos.x += vel.dx;
    pos.y += vel.dy;
    pos.z += vel.dz;
});

// 迭代器方式
for (auto [entity, pos, vel] : view) {
    pos.x += vel.dx;
}
```

### 移除实体和组件

```cpp
// 移除所有组件（销毁实体）
registry.destroy(player);

// 遍历所有实体
registry.each([](Entity e) {
    std::cout << "Entity ID: " << e.index() << std::endl;
});
```

---

## 🔧 核心概念

### 实体 (Entity)

实体是唯一标识符，本身不包含数据：

```cpp
Entity entity = registry.create();
uint32 id = entity.index();      // 实体索引
uint32 version = entity.version(); // 版本号

// 验证实体
if (registry.valid(entity)) {
    // 实体有效
}
```

### 组件 (Component)

组件是纯数据结构，不包含逻辑：

```cpp
// 推荐使用 POD 类型
struct Transform {
    float x, y, z;
    float rotation;
};

struct Renderable {
    int meshId;
    int textureId;
    bool visible;
};

// 组件应该只包含数据，逻辑放在系统中
```

### 系统 (System)

系统是处理逻辑的地方：

```cpp
#include "common/ecs/system.h"

class MovementSystem : public System {
public:
    void update(Registry& registry, float deltaTime) override {
        auto view = registry.view<Position, Velocity>();
        view.each([deltaTime](Entity e, Position& pos, Velocity& vel) {
            pos.x += vel.dx * deltaTime;
            pos.y += vel.dy * deltaTime;
            pos.z += vel.dz * deltaTime;
        });
    }

    const char* name() const override { return "MovementSystem"; }
    int priority() const override { return 0; }
};

// 使用系统执行器
SystemExecutor executor;
executor.addSystem(std::make_unique<MovementSystem>());
executor.execute(registry, deltaTime);
```

### Archetype

Archetype 是 RenduCore 的核心优化机制：

```
Archetype<Position, Velocity>:
┌─────────────────────────────────────┐
│ entityIds:    [e1, e2, e3, ...]   │
│ Position.x:   [0,  0,  0,  ...]   │  ← SOA: 按列存储
│ Position.y:   [0,  0,  0,  ...]   │
│ Position.z:   [0,  0,  0,  ...]   │
│ Velocity.dx:  [1,  1,  1,  ...]   │
│ Velocity.dy:  [0.5,0.5,0.5, ...]  │
│ Velocity.dz:  [0,  0,  0,  ...]   │
└─────────────────────────────────────┘
```

相同组件组合的实体存储在同一个 Archetype 中，实现极致的缓存性能。

---

## 📚 更多文档

- [核心概念](CONCEPTS.md) - 深入理解 ECS 架构
- [示例代码](EXAMPLES.md) - 更多使用场景
- [API 参考](API/) - 完整 API 文档
- [性能优化](PERFORMANCE.md) - 性能优化技巧
