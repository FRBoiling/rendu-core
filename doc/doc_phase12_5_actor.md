# 阶段 12.5: Core 层 - Actor 系统 (actor)

## 目标
实现轻量级 Actor 模型，提供高并发、消息驱动的并发编程能力。

---

## 设计理念

### 1. Actor 模型核心
- **隔离**: 每个 Actor 独立执行，共享内存隔离
- **消息传递**: 通过异步消息通信，无锁设计
- **容错**: Actor 可独立崩溃，不影响其他 Actor

### 2. 与框架集成
- 复用 io_context 进行调度
- 复用 event_bus 进行消息传递
- 复用 serializer 进行消息序列化
- 复用 logger 进行日志记录

### 3. 与 ECS 共存
- Actor: 高并发、隔离、消息驱动
- ECS: 密集计算、数据导向、批量处理
- 两者可以互补，互不干扰

---

## 任务清单

### 1. actor.h
**文件路径**: `src/core/include/core/actor/actor.h`

**职责**:
- Actor 抽象基类
- 定义 Actor 生命周期
- 消息处理接口

**核心接口**:
```cpp
class Actor {
public:
    using ActorId = uint64_t;
    using MessageHandler = std::function<void(const Message&)>;

    Actor();
    virtual ~Actor();

    // Actor ID
    ActorId GetId() const;

    // Actor 名称
    virtual std::string GetName() const = 0;

    // 消息处理入口
    virtual void Receive(const Message& msg) = 0;

    // 消息处理器注册
    template<typename MessageType, typename Handler>
    void RegisterHandler(Handler&& handler);

    // 初始化
    virtual void Initialize() {}

    // 启动
    virtual void Start() {}

    // 停止
    virtual void Stop() {}

    // 销毁
    virtual void Destroy() {}

protected:
    // 获取 Actor 系统
    ActorSystem& GetActorSystem();

    // 发送消息给自己
    void Self(const Message& msg);

private:
    ActorId id_;
    std::unordered_map<size_t, MessageHandler> handlers_;
};
```

---

### 2. actor_system.h
**文件路径**: `src/core/include/core/actor/actor_system.h`

**职责**:
- Actor 生命周期管理
- Actor 调度
- Actor 注册表
- 分布式支持（预留）

**核心接口**:
```cpp
class ActorSystem {
public:
    // 单例
    static ActorSystem& Instance();

    // 初始化
    bool Initialize(int worker_threads = 0);

    // 启动
    void Start();

    // 停止
    void Stop();

    // 注册 Actor
    ActorId Register(std::shared_ptr<Actor> actor);

    // 注销 Actor
    void Unregister(ActorId id);

    // 获取 Actor
    std::shared_ptr<Actor> Get(ActorId id);

    // 查找 Actor
    std::shared_ptr<Actor> Find(const std::string& name);

    // 创建 Actor
    template<typename ActorType, typename... Args>
    std::shared_ptr<ActorType> Create(Args&&... args);

    // 停止 Actor
    void StopActor(ActorId id);

    // 获取 io_context
    common::IOContext& GetIOContext();

private:
    std::unordered_map<ActorId, std::shared_ptr<Actor>> actors_;
    std::unique_ptr<common::IOContext> io_context_;
    std::vector<std::thread> workers_;
    bool running_;
};
```

---

### 3. actor_ref.h
**文件路径**: `src/core/include/core/actor/actor_ref.h`

**职责**:
- Actor 弱引用
- 跨进程消息传递
- 序列化支持

**核心接口**:
```cpp
class ActorRef {
public:
    ActorRef();
    explicit ActorRef(ActorId id);
    explicit ActorRef(const std::string& name);

    // 发送消息
    void Tell(const Message& msg);

    // 异步发送并等待回复
    template<typename ReplyType>
    std::future<ReplyType> Ask(const Message& msg);

    // 获取 ID
    ActorId GetId() const;

    // 获取名称
    std::string GetName() const;

    // 是否有效
    bool IsValid() const;

    // 序列化
    std::string Serialize() const;

    // 反序列化
    static ActorRef Deserialize(const std::string& data);

private:
    ActorId id_;
    std::string name_;
};
```

---

### 4. message.h
**文件路径**: `src/core/include/core/actor/message.h`

**职责**:
- 消息定义
- 消息类型注册
- 消息序列化

**核心接口**:
```cpp
class Message {
public:
    using MessageType = size_t;

    Message(MessageType type);
    virtual ~Message() = default;

    // 获取消息类型
    MessageType GetType() const;

    // 获取发送者
    ActorId GetSender() const;

    // 获取接收者
    ActorId GetReceiver() const;

    // 设置发送者
    void SetSender(ActorId id);

    // 设置接收者
    void SetReceiver(ActorId id);

    // 序列化
    virtual std::string Serialize() const;

    // 反序列化
    virtual void Deserialize(const std::string& data);

protected:
    MessageType type_;
    ActorId sender_;
    ActorId receiver_;
};

// 消息类型 ID 生成
template<typename T>
size_t GetMessageType() {
    static size_t id = typeid(T).hash_code();
    return id;
}

// 消息构建器
template<typename T>
class MessageBuilder {
public:
    template<typename... Args>
    static std::shared_ptr<T> Create(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
};
```

---

### 5. message_queue.h
**文件路径**: `src/core/include/core/actor/message_queue.h`

**职责**:
- Actor 消息队列
- 支持优先级
- 支持限流
- 无锁设计（可选）

**核心接口**:
```cpp
class MessageQueue {
public:
    // 入队
    void Push(std::shared_ptr<Message> msg, int priority = 0);

    // 出队（阻塞）
    std::shared_ptr<Message> Pop();

    // 出队（非阻塞）
    std::shared_ptr<Message> TryPop();

    // 获取队列大小
    size_t Size() const;

    // 是否为空
    bool Empty() const;

    // 清空队列
    void Clear();

    // 设置最大容量
    void SetMaxCapacity(size_t capacity);

    // 设置丢弃策略
    void SetDropPolicy(DropPolicy policy);
};

enum class DropPolicy {
    DiscardOldest,    // 丢弃最旧的
    DiscardNewest,    // 丢弃最新的
    Block             // 阻塞
};
```

---

### 6. actor_pool.h (可选)
**文件路径**: `src/core/include/core/actor/actor_pool.h`

**职责**:
- Actor 对象池
- 复用 Actor 实例
- 减少内存分配

**核心接口**:
```cpp
template<typename ActorType>
class ActorPool {
public:
    // 获取 Actor
    std::shared_ptr<ActorType> Acquire();

    // 归还 Actor
    void Release(std::shared_ptr<ActorType> actor);

    // 设置最大容量
    void SetMaxCapacity(size_t capacity);

    // 获取当前数量
    size_t GetSize() const;

    // 清空池
    void Clear();
};
```

---

## 设计要点

### 1. 消息传递模型
- **Tell**: 发后即忘，不等待回复
- **Ask**: 异步请求，等待回复（Future）
- **Broadcast**: 广播给多个 Actor

### 2. 并发控制
- 每个 Actor 单线程处理消息
- Actor 之间无共享状态
- 通过 io_context 线程池调度

### 3. 容错机制
- Actor 崩溃不影响其他 Actor
- 支持 Actor 监控和重启
- 支持 Actor 超时处理

### 4. 分布式支持（预留）
- ActorRef 支持序列化
- 消息支持序列化
- 为跨进程通信预留接口

### 5. 性能优化
- 消息零拷贝
- 无锁队列（可选）
- Actor 对象池
- 批量消息处理

---

## 典型使用场景

### 1. 网络连接管理
```cpp
class ConnectionActor : public Actor {
public:
    std::string GetName() const override { return "ConnectionActor"; }

    void Receive(const Message& msg) override {
        if (msg.GetType() == GetMessageType<DataMessage>()) {
            // 处理数据
        }
    }
};
```

### 2. 任务调度
```cpp
class TaskSchedulerActor : public Actor {
public:
    std::string GetName() const override { return "TaskSchedulerActor"; }

    void Receive(const Message& msg) override {
        if (msg.GetType() == GetMessageType<TaskMessage>()) {
            // 分发任务
        }
    }
};
```

### 3. 状态管理
```cpp
class StateActor : public Actor {
public:
    std::string GetName() const override { return "StateActor"; }

    void Receive(const Message& msg) override {
        // 维护状态，响应查询
    }
};
```

---

## 验收标准

- [ ] Actor 创建/销毁正常
- [ ] 消息传递正确（Tell/Ask）
- [ ] 多线程安全
- [ ] ActorRef 可序列化
- [ ] 消息队列支持优先级
- [ ] 性能满足预期（10K msg/s per actor）
- [ ] 单元测试覆盖

---

## 依赖
- 阶段 2 (io) - io_context 调度
- 阶段 3 (log) - 日志记录
- 阶段 5 (ser) - 消息序列化
- 阶段 6 (event) - 事件通知
- RenduCore::common - 公共层
