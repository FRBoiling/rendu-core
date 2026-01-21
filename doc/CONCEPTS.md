# ECS (Entity Component System) 封装说明

## 概述

本项目对 EnTT 库进行了封装,实现了完全的头文件隔离:

1. ✅ **完全头文件隔离**: `.h` 文件不包含任何 EnTT 头文件
2. ✅ **Pimpl 模式**: 使用实现指针隐藏 EnTT 细节
3. ✅ **类型安全**: 提供强类型的 Entity 和 Component 接口
4. ✅ **完整功能**: 支持实体管理、组件管理、视图迭代等

## 核心设计

### 头文件隔离原则

**核心原则**: EnTT 库只在 `.cpp` 文件中包含，绝对不在 `.h` 文件中包含。

```
registry.h     - 完全不包含 EnTT，只有模板声明
registry.cpp   - 包含 <entt/entt.hpp>，实现所有模板
entity.h       - 不包含 EnTT，简单的 Entity 包装
```

## 目录结构

```
src/common/include/common/ecs/
├── entity.h       # Entity 类定义(不包含 EnTT)
└── registry.h     # Registry 类定义(不包含 EnTT)

src/common/src/ecs/
└── registry.cpp   # Registry 实现(包含 EnTT)
```

## 核心类

### 1. Entity (实体)

```cpp
#include "common/ecs/entity.h"

// 创建实体
Entity e1;                          // 无效实体
Entity e2(uint32_t(123));          // 从整数值构造

// 实体操作
bool valid = e2.valid();            // 检查是否有效
uint32_t value = e2.value();        // 获取值
```

### 2. Registry (注册中心)

```cpp
#include "common/ecs/registry.h"

// 创建注册中心
Registry registry;

// 实体管理
Entity entity = registry.create();           // 创建单个实体
auto entities = registry.createMany(10);     // 批量创建
registry.destroy(entity);                    // 销毁实体
registry.clear();                            // 清空所有
registry.reserve(100);                      // 预留容量

// 组件管理
struct Position : public Component {
    float x, y;
};

// 添加组件
registry.emplace<Position>(entity, 10.0f, 20.0f);

// 获取组件
Position* pos = registry.tryGet<Position>(entity);

// 检查组件
if (registry.has<Position>(entity)) {
    // 实体拥有 Position 组件
}

// 获取或添加组件
Position& pos = registry.getOrEmplace<Position>(entity);

// 移除组件
registry.remove<Position>(entity);

// 设置回调
registry.onConstruct<Position>([](Entity e, Position& pos) {
    printf("Position added to entity %u\n", e.value());
});

registry.onDestroy<Position>([](Entity e, Position& pos) {
    printf("Position removed from entity %u\n", e.value());
});
```

### 3. View (视图)

```cpp
#include "common/ecs/registry.h"

// 创建视图 - 遍历拥有特定组件的实体
auto view = registry.view<Position, Velocity>();

// 检查视图状态
if (view.empty()) {
    printf("No entities found\n");
}

printf("View size: %zu\n", view.size());

// 遍历视图中的实体
view.each([](Entity entity, Position& pos, Velocity& vel) {
    pos.x += vel.vx;
    pos.y += vel.vy;
});

// 遍历所有实体
registry.each([](Entity entity) {
    printf("Entity: %u\n", entity.value());
});
```

## 完整示例

```cpp
#include "common/ecs/registry.h"
#include <cstdio>

using namespace Rendu;

// 定义组件
struct Position : public Component {
    float x, y;
    Position(float x = 0, float y = 0) : x(x), y(y) {}
};

struct Velocity : public Component {
    float vx, vy;
    Velocity(float vx = 0, float vy = 0) : vx(vx), vy(vy) {}
};

struct Name : public Component {
    const char* name;
    Name(const char* name) : name(name) {}
};

int main() {
    Registry registry;

    // 创建实体并添加组件
    Entity player = registry.create();
    registry.emplace<Position>(player, 100.0f, 200.0f);
    registry.emplace<Velocity>(player, 1.5f, 0.8f);
    registry.emplace<Name>(player, "Player");

    Entity enemy = registry.create();
    registry.emplace<Position>(enemy, 50.0f, 50.0f);
    registry.emplace<Velocity>(enemy, -0.5f, -0.3f);
    registry.emplace<Name>(enemy, "Enemy");

    // 使用视图更新所有实体
    auto view = registry.view<Position, Velocity>();
    view.each([&](Entity e, Position& pos, Velocity& vel) {
        pos.x += vel.vx;
        pos.y += vel.vy;
    });

    // 使用视图查询名称和位置
    auto nameView = registry.view<Name, Position>();
    nameView.each([&](Entity e, Name& name, Position& pos) {
        printf("%s is at (%.1f, %.1f)\n", name.name, pos.x, pos.y);
    });

    return 0;
}
```

## 设计细节

### 1. Pimpl 模式

```cpp
// registry.h - 完全不包含 EnTT
class Registry {
private:
    struct Impl;  // 前向声明
    std::unique_ptr<Impl> m_impl;
};

// registry.cpp - 包含 EnTT 并定义 Impl
#include <entt/entt.hpp>

struct Registry::Impl {
    entt::registry registry;  // EnTT 完全隐藏在这里
};
```

### 2. 模板实现

所有模板函数的实现都在 `registry.cpp` 中，使用 EnTT 实现：

```cpp
// registry.cpp
template <typename ComponentType, typename... Args>
ComponentType& Registry::emplace(Entity entity, Args&&... args)
{
    auto& comp = m_impl->registry.emplace<ComponentType>(
        static_cast<entt::entity>(entity.value()),
        std::forward<Args>(args)...
    );
    return comp;
}
```

### 3. View 实现

View 类使用 `void*` 指针存储 EnTT 视图，实现类型擦除：

```cpp
// registry.h
template <typename... ComponentTypes>
class Registry::View {
private:
    void* m_viewPtr;
    bool m_isConst;
public:
    template <typename Func>
    void each(Func&& func) const;
};

// registry.cpp
template <typename... ComponentTypes>
template <typename Func>
void Registry::View<ComponentTypes...>::each(Func&& func) const
{
    auto* view = static_cast<entt::basic_view<entt::entity, ComponentTypes...>*>(m_viewPtr);
    view->each([&func](entt::entity e, ComponentTypes&... comps) {
        func(Entity(static_cast<uint32_t>(e)), comps...);
    });
}
```

## 编译说明

### CMake 配置

```cmake
find_package(EnTT REQUIRED)
target_link_libraries(common PRIVATE EnTT::EnTT)
```

### 模板编译注意事项

由于模板函数在 `.cpp` 文件中实现，编译器需要在使用时看到模板的定义。有以下几种解决方案：

**方案 1: 包含实现文件**

在用户代码中包含 `registry.cpp`（不推荐）：

```cpp
#include "common/ecs/registry.cpp"  // 临时方案
```

**方案 2: 显式实例化（推荐）**

在 `registry.cpp` 中为常用组件类型显式实例化模板：

```cpp
// registry.cpp
template Position& Registry::emplace<Position>(Entity, Position&&);
template void Registry::remove<Position>(Entity);
template Position* Registry::tryGet<Position>(Entity);
// ... 其他实例化
```

**方案 3: 将模板声明为 inline**

将模板函数声明为 `inline` 并在头文件中提供默认实现。

## 优势

1. **完全隔离**: EnTT 完全隐藏在 `.cpp` 文件中
2. **可替换**: 未来可以替换为其他 ECS 库而不影响用户代码
3. **类型安全**: 提供强类型的 Entity 和 Component 接口
4. **完整功能**: 支持实体管理、组件管理、视图迭代、回调等

## 注意事项

1. **继承 Component**: 所有组件必须继承自 `Rendu::Component` 基类
2. **模板编译**: 模板函数在 `.cpp` 中实现，需要考虑编译策略
3. **性能**: 封装层有少量开销，但对性能影响极小
4. **线程安全**: 当前实现不是线程安全的，需要外部同步

## 参考

- [EnTT 官方文档](https://github.com/skypjack/entt)
- [ECS 架构模式](https://github.com/SanderMertens/ecs-faq)
