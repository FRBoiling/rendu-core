# 跨节点 Actor 通信实现计划

**创建日期**: 2026-02-02
**状态**: 🚧 进行中
**预计完成**: 2026-02-17

---

## 一、目标

实现跨节点 Actor 通信，支持分布式系统中不同节点上的 Actor 互相发送消息。

---

## 二、架构设计

### 2.1 通信模型

```
节点 A (Node A)                    节点 B (Node B)
+------------------+              +------------------+
|  ActorSystem     |              |  ActorSystem     |
|  +----------+   |   TCP       |   +----------+   |
|  | Actor1   | <------------> |   | Actor1   |   |
|  +----------+   |             |   +----------+   |
|                  RemoteChannel                  |
+------------------+              +------------------+
```

### 2.2 核心组件

| 组件 | 职责 |
|------|--------|
| RemoteChannel | 封装网络通信，提供消息级别的接口 |
| RemoteMessage/RemoteRequest/RemoteResponse | Protobuf 消息定义 |
| RemoteActorSystem | 扩展 ActorSystem，支持远程消息路由 |

---

## 三、实现步骤

### 阶段 1: 基础设施 ✅ 已开始

#### 1.1 Protobuf 消息定义 ✅
- 文件: `src/apps/protocol/remote_message.proto`
- 定义:
  - `RemoteActorRef` - ActorRef 的 protobuf 表示
  - `RemoteMessage` - Tell 模式消息
  - `RemoteRequest` - Ask 模式请求
  - `RemoteResponse` - Ask 模式响应
  - `RemotePacket` - 统一封装

#### 1.2 RemoteChannel 实现 ✅ 已完成
- 头文件: `src/common/include/common/net/remote_channel.h`
- 源文件: `src/common/src/net/remote_channel.cpp`
- 功能:
  - TCP 连接管理 ✅
  - 自动重连机制 ✅
  - 消息序列化/反序列化 ✅
  - 消息分发（Message/Request/Response）✅

**待完成**:
- [x] 完善 RemoteChannel 实现
- [ ] 集成 protobuf 编译（需要构建验证）
- [ ] 取消注释 protobuf 头文件引用

---

### 阶段 2: RemoteActorSystem 🚧 进行中

#### 2.1 扩展 ActorSystem
- 文件: `src/core/include/core/actor/remote_actor_system.h` ✅
- 文件: `src/core/src/actor/remote_actor_system.cpp` ✅
- 功能:
  - 节点 ID 管理 ✅
  - RemoteChannel 管理池 ✅
  - 路由决策（本地 vs 远程）✅
  - 请求 ID 生成和匹配 ✅

**待完成**:
- [ ] IoContext 获取方式设计
- [ ] 取消注释 protobuf 相关代码
- [ ] 实现 Message 序列化/反序列化

#### 2.2 实现细节
```cpp
class RemoteActorSystem : public ActorSystem {
public:
    struct Config {
        std::string node_id;
        std::string listen_address;
        uint16_t listen_port;
        std::vector<std::string> known_nodes;
    };

    explicit RemoteActorSystem(const Config& config);

    // 覆盖 ActorSystem 方法
    void tell(const ActorRef& target, std::shared_ptr<Message> msg) override;
    std::shared_ptr<Message> ask(const ActorRef& target,
                                  std::shared_ptr<Message> msg,
                                  uint64_t timeout_ms = 0) override;

private:
    // 节点管理
    std::string node_id_;
    std::unordered_map<std::string, std::shared_ptr<RemoteChannel>> node_channels_;

    // 请求管理
    std::atomic<uint64_t> next_request_id_{1};
    std::unordered_map<uint64_t, std::shared_ptr<MessagePromise>> pending_requests_;

    // 本地 Actor 路由（使用 ActorRef 路径判断）
    bool is_local_actor(const ActorRef& ref) const;
};
```

---

### 阶段 3: 集成和测试 ⏳ 待实现

#### 3.1 单元测试
- 文件: `src/tests/common/net/remote_channel_test.cpp`
- 测试:
  - 连接/断开
  - 消息发送/接收
  - 自动重连
  - 并发访问

#### 3.2 集成测试
- 文件: `src/tests/core/actor/remote_actor_system_test.cpp`
- 测试:
  - 本地消息路由
  - 远程消息发送
  - Ask 模式（跨节点）
  - 故障恢复

---

## 四、设计原则

1. **优先使用框架已有功能**
   - 使用 `TcpSocket` 和 `Channel`（网络层）
   - 使用 `ProtobufSerializer`（序列化）
   - 使用 `ActorRef`（已支持序列化）
   - 使用 `IoContext`（异步 I/O）

2. **最小化新代码**
   - RemoteChannel 只封装网络通信
   - RemoteActorSystem 只扩展路由逻辑
   - 不重复实现 Actor 系统核心

3. **渐进式实现**
   - 先实现基本连接和消息传递
   - 再添加重连和错误处理
   - 最后优化性能

---

## 五、已知风险

| 风险 | 应对措施 |
|------|---------|
| 消息序列化复杂度 | 使用 ActorRef.serialize() 简化 |
| 请求-响应匹配 | 使用原子 ID 生成器 |
| 网络断开处理 | RemoteChannel 自动重连 |
| 并发安全性 | 使用互斥锁保护共享状态 |

---

## 六、当前进度

- [x] 阶段 1.1: Protobuf 消息定义
- [x] 阶段 1.2: RemoteChannel 实现（核心逻辑完成）
- [ ] 阶段 1.3: 编译 protobuf 生成代码（待构建）
- [ ] 阶段 2.1: RemoteActorSystem 实现（接口和核心逻辑完成，待完善 protobuf 集成）
- [ ] 阶段 2.2: IoContext 获取方式设计（待实现）
- [ ] 阶段 3: 集成和测试（待实现）

**进度**: 50%

---

## 七、下一步

1. 设计 IoContext 的获取方式（RemoteActorSystem 需要访问 IoContext）
2. 构建项目生成 protobuf 代码
3. 取消注释所有 protobuf 相关代码
4. 实现 Message 序列化/反序列化
5. 编写 RemoteChannel 单元测试
6. 编写 RemoteActorSystem 集成测试

## 八、技术债务

- [ ] 延迟重连需要 timer 支持
- [ ] Protobuf 编译需要集成到构建系统
- [ ] 需要完整的单元测试覆盖

---

**文档版本**: v1.0
**最后更新**: 2026-02-02
