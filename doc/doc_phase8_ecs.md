# 阶段 8: Common 层 - ECS 系统 (ecs)

## 目标
- 基于 entt 的 ECS 封装
- 提供简化的 API
- 支持系统执行顺序控制

---

## 文件结构

```
src/common/
├── include/common/ecs/
│   ├── registry.h
│   ├── system.h
│   └── view.h
└── src/ecs/
    ├── registry.cpp
    ├── system.cpp
    └── view.cpp
```

---

## 任务清单

### 1. 实体注册表 (registry.h/cpp)
- [ ] 封装 entt::registry
- [ ] 提供便捷接口
- [ ] 实体/组件增删改

### 2. 系统抽象 (system.h/cpp)
- [ ] System 接口
- [ ] 更新顺序控制
- [ ] 系统依赖管理

### 3. 视图查询 (view.h/cpp)
- [ ] 封装 entt::view
- [ ] 简化查询接口

---

## 头文件设计

### registry.h
```cpp
#pragma once

#include <entt/entt.hpp>
#include <vector>

namespace rendu::ecs {

using Entity = entt::entity;

class Registry {
public:
    Registry() = default;

    // 创建实体
    Entity create();
    void destroy(Entity entity);

    // 组件操作
    template<typename Component, typename... Args>
    Component& emplace(Entity entity, Args&&... args) {
        return registry_.emplace<Component>(entity, std::forward<Args>(args)...);
    }

    template<typename Component>
    Component& get(Entity entity) {
        return registry_.get<Component>(entity);
    }

    template<typename Component>
    const Component& get(Entity entity) const {
        return registry_.get<Component>(entity);
    }

    template<typename Component>
    bool has(Entity entity) const {
        return registry_.all_of<Component>(entity);
    }

    template<typename Component>
    void remove(Entity entity) {
        registry_.remove<Component>(entity);
    }

    // 访问底层 registry
    entt::registry& native();
    const entt::registry& native() const;

private:
    entt::registry registry_;
};

} // namespace rendu::ecs
```

### system.h
```cpp
#pragma once

#include <string>
#include <memory>
#include <common/ecs/registry.h>

namespace rendu::ecs {

class System {
public:
    System() = default;
    virtual ~System() = default;

    virtual void update(float delta_time) = 0;
    virtual std::string name() const = 0;
    virtual int priority() const { return 0; } // 越小越先执行
};

class SystemManager {
public:
    void add_system(std::shared_ptr<System> system);
    void remove_system(const std::string& name);

    void update(float delta_time);

private:
    std::vector<std::shared_ptr<System>> systems_;
};

} // namespace rendu::ecs
```

### view.h
```cpp
#pragma once

#include <entt/entt.hpp>
#include <functional>
#include <common/ecs/registry.h>

namespace rendu::ecs {

template<typename... Components>
class View {
public:
    using entt_view = entt::view<entt::get_t<Components...>>;

    View(entt::registry& registry)
        : view_(registry.view<Components...>()) {}

    template<typename Func>
    void for_each(Func func) {
        view_.each(func);
    }

private:
    entt_view view_;
};

} // namespace rendu::ecs
```

---

## 单元测试

### 测试文件
```
src/tests/common/ecs/
├── CMakeLists.txt
├── registry_test.cpp
├── system_test.cpp
└── view_test.cpp
```

### registry_test.cpp
```cpp
#include <catch2/catch_test_macros.hpp>
#include <common/ecs/registry.h>

using namespace rendu::ecs;

struct Position {
    float x, y;
};

struct Velocity {
    float vx, vy;
};

TEST_CASE("Registry create and destroy", "[ecs][registry]") {
    Registry registry;
    Entity entity = registry.create();

    REQUIRE(registry.valid(entity));

    registry.destroy(entity);
    REQUIRE(!registry.valid(entity));
}

TEST_CASE("Registry components", "[ecs][registry]") {
    Registry registry;
    Entity entity = registry.create();

    registry.emplace<Position>(entity, 1.0f, 2.0f);
    REQUIRE(registry.has<Position>(entity));

    auto& pos = registry.get<Position>(entity);
    REQUIRE(pos.x == 1.0f);
    REQUIRE(pos.y == 2.0f);
}
```

---

## 验收标准

### 功能
- [ ] 实体组件增删改正常
- [ ] 系统执行顺序可控
- [ ] 视图查询正确

### 性能
- [ ] 支持 10k+ 实体
- [ ] 组件访问延迟 < 100ns

---

## 下一步
完成本阶段后，进入 **阶段 9: Core 层 - 引擎核心 (engine)**
