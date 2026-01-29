# 阶段 13: 应用层 - 示例程序

## 概述

本阶段创建了多个示例程序，演示框架各核心模块的使用方法，包括：
- 日志 (Logger)
- Actor 模型
- 引擎 (Engine)
- 事件系统 (Event)
- ECS (实体组件系统)
- 网络通信 (Network)

## 文件结构

```
src/apps/example/
├── CMakeLists.txt              # 构建配置
├── example_messages.h          # 自定义消息定义
├── log_example.cpp             # 日志使用示例
├── actor_example.cpp           # Actor 模型示例
├── engine_example.cpp          # 引擎使用示例
├── event_example.cpp           # 事件系统示例
├── ecs_example.cpp             # ECS 使用示例
└── net_example.cpp             # 网络通信示例
```

## 示例程序说明

### 1. log_example.cpp

演示日志系统的使用：
- 创建不同级别的 Logger
- 添加控制台和文件 Sink
- 使用自定义格式化器
- 结构化日志（带键值字段）
- 多线程日志记录
- 动态调整日志级别

### 2. actor_example.cpp

演示 Actor 并发模型：
- 创建 Ping 和 Pong Actor
- 演示 Tell 模式（异步消息传递）
- 演示 Ask 模式（同步等待响应）
- 消息队列和线程池
- Actor 生命周期管理

### 3. engine_example.cpp

演示引擎框架的使用：
- 初始化引擎
- 访问引擎上下文
- 启动/停止引擎
- 使用 Logger 记录日志
- 自定义 Engine 子类扩展

### 4. event_example.cpp

演示事件系统：
- 定义自定义事件（用户登录/登出、订单创建等）
- 创建事件处理器
- 同步/异步事件发布
- 订阅/取消订阅事件
- 事件类型查询

### 5. ecs_example.cpp

演示 ECS（实体组件系统）：
- 定义组件（位置、速度、生命值等）
- 创建和管理实体
- 实现系统（移动、战斗、统计等）
- 组件查询和视图
- 组件添加/移除/修改

### 6. net_example.cpp

演示网络通信：
- 自定义编解码器（文本、长度前缀）
- 编解码演示
- TCP Socket 使用（伪代码示例）
- 异步连接、发送、接收

## 构建和运行

### 构建所有示例

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### 运行各个示例

```bash
# 日志示例
./build/src/apps/example/log_example

# Actor 示例
./build/src/apps/example/actor_example

# 引擎示例
./build/src/apps/example/engine_example

# 事件示例
./build/src/apps/example/event_example

# ECS 示例
./build/src/apps/example/ecs_example

# 网络示例
./build/src/apps/example/net_example
```

## 验收标准

### 功能
- [x] 所有示例可正常编译
- [x] 代码结构清晰，易于理解
- [x] 注释详细，说明各功能点
- [x] 输出信息清晰，便于理解

### 可用性
- [x] 代码易于理解和学习
- [x] 注释清晰
- [x] 可作为开发模板使用
- [x] 演示了框架的主要功能

## 技术要点

1. **日志系统**：异步日志记录、多 Sink 支持、结构化日志
2. **Actor 模型**：消息驱动并发、Tell/Ask 模式、线程池调度
3. **引擎框架**：上下文管理、生命周期控制
4. **事件系统**：发布-订阅模式、同步/异步分发、类型安全
5. **ECS**：实体组件系统、视图查询、系统执行
6. **网络通信**：异步 IO、自定义编解码、连接管理

## 下一步

完成本阶段后，进入 **阶段 14: 应用层 - 服务器 (server)**
