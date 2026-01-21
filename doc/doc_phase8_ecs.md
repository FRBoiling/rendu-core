# 阶段 8: Common 层 - ECS 系统 (ecs)

## 目标
基于 entt 的 ECS 封装，提供简化的 API。

---

## 任务清单

### 1. registry.h
**文件路径**: `src/common/include/common/ecs/registry.h`

**职责**:
- 实体注册表封装
- 管理实体和组件

**核心接口**:
```cpp
class Registry {
public:
    // 创建实体
    Entity Create();

    // 销毁实体
    void Destroy(Entity entity);

    // 添加组件
    template<typename Component, typename... Args>
    Component& AddComponent(Entity entity, Args&&... args);

    // 移除组件
    template<typename Component>
    void RemoveComponent(Entity entity);

    // 获取组件
    template<typename Component>
    Component& GetComponent(Entity entity);

    // 判断是否有组件
    template<typename Component>
    bool HasComponent(Entity entity);

    // 创建视图
    template<typename... Components>
    auto View() -> entt::view<entt::exclude_t<>, Components...>;
};
```

---

### 2. system.h
**文件路径**: `src/common/include/common/ecs/system.h`

**职责**:
- 系统抽象
- 定义系统接口

**核心接口**:
```cpp
class ISystem {
public:
    virtual ~ISystem() = default;

    // 系统名称
    virtual std::string GetName() const = 0;

    // 更新
    virtual void Update(float delta_time) = 0;

    // 初始化
    virtual void Initialize() {}

    // 清理
    virtual void Shutdown() {}
};

// 系统基类
class SystemBase : public ISystem {
public:
    SystemBase(Registry& registry);

    Registry& GetRegistry();

protected:
    Registry& registry_;
};
```

---

### 3. view.h
**文件路径**: `src/common/include/common/ecs/view.h`

**职责**:
- 视图查询封装
- 提供便利的迭代接口

**核心接口**:
```cpp
template<typename... Components>
class View {
public:
    // 遍历实体
    template<typename Callback>
    void ForEach(Callback&& callback);

    // 获取实体数量
    size_t Size() const;

    // 判断是否为空
    bool Empty() const;
};
```

---

## 设计要点

### 1. 实体管理
- 使用 entt::entity 作为实体类型
- 实体 ID 自增和回收

### 2. 组件管理
- 组件是普通数据结构
- 支持多个相同组件（可选）
- 组件存储优化（SoA）

### 3. 系统
- 系统处理特定组件组合
- 系统按优先级更新
- 支持系统依赖

### 4. 性能优化
- 缓存友好布局
- 批量处理
- 并行处理（可选）

---

## 验收标准

- [ ] 实体创建/销毁正常
- [ ] 组件增删改正确
- [ ] 系统执行顺序正确
- [ ] 性能满足预期
- [ ] 单元测试覆盖

---

## 依赖
- 阶段 6 (event) - 事件通知
- 3rdparty::entt - ECS 库
