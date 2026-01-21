# 阶段 6: Common 层 - 事件系统 (event)

## 目标
实现事件总线机制，支持事件订阅和分发。

---

## 任务清单

### 1. event.h
**文件路径**: `src/common/include/common/event/event.h`

**职责**:
- 事件基类定义
- 事件类型 ID

**核心接口**:
```cpp
class Event {
public:
    virtual ~Event() = default;

    // 获取事件类型 ID
    virtual size_t GetType() const = 0;

    // 获取事件名称
    virtual std::string GetName() const = 0;
};

// 事件类型 ID 生成器
template<typename T>
size_t GetEventType() {
    static size_t id = typeid(T).hash_code();
    return id;
}
```

---

### 2. event_bus.h
**文件路径**: `src/common/include/common/event/event_bus.h`

**职责**:
- 事件总线，管理订阅者
- 事件的发布和分发
- 支持同步和异步分发

**核心接口**:
```cpp
class EventBus {
public:
    // 单例获取
    static EventBus& Instance();

    // 订阅事件
    template<typename EventType, typename Callback>
    void Subscribe(Callback&& callback);

    // 取消订阅
    template<typename EventType>
    void Unsubscribe();

    // 发布事件（同步）
    template<typename EventType>
    void Publish(const EventType& event);

    // 发布事件（异步）
    template<typename EventType>
    void PublishAsync(const EventType& event);
};
```

---

### 3. handler.h
**文件路径**: `src/common/include/common/event/handler.h`

**职责**:
- 事件处理器接口
- 基类定义

**核心接口**:
```cpp
class IEventHandler {
public:
    virtual ~IEventHandler() = default;

    // 处理事件
    virtual void Handle(const Event& event) = 0;

    // 获取感兴趣的事件类型
    virtual std::vector<size_t> GetEventTypes() const = 0;
};

// 事件处理器模板基类
template<typename EventType>
class EventHandler : public IEventHandler {
public:
    virtual void OnEvent(const EventType& event) = 0;

    void Handle(const Event& event) override;

    std::vector<size_t> GetEventTypes() const override;
};
```

---

## 设计要点

### 1. 发布订阅模式
- 解耦事件生产者和消费者
- 支持多个订阅者

### 2. 同步/异步分发
- 同步：立即执行
- 异步：投递到 io_context

### 3. 类型安全
- 使用模板确保类型安全
- 编译期检查

### 4. 性能优化
- 使用 unordered_map 快速查找
- 避免动态类型转换

---

## 验收标准

- [ ] 事件订阅/发布正确
- [ ] 同步分发顺序正确
- [ ] 异步分发不阻塞
- [ ] 取消订阅正常工作
- [ ] 性能满足预期
- [ ] 单元测试覆盖

---

## 依赖
- 阶段 2 (io) - io_context 异步分发
- 阶段 3 (log) - 日志记录
- 阶段 1 (util) - 基础工具
