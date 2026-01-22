# RenduCore 架构设计文档

## 一、整体架构层次

```
┌─────────────────────────────────────────────────────┐
│                   Apps (应用层)                        │
│  示例程序、服务器、客户端、工具                        │
└────────────────┬────────────────────────────────────┘
                 │
┌────────────────▼─────────────────────────────────────┐
│                   Core (核心层)                        │
│  框架核心逻辑、引擎、生命周期管理                       │
│  - Engine: 主引擎                                     │
│  - Loop: 主循环                                       │
│  - State: 状态管理                                    │
│  - Actor: Actor 模型                                   │
│  - Lifecycle: 生命周期                                 │
│  依赖: Common                                          │
└────────────────┬────────────────────────────────────┘
                 │
┌────────────────▼─────────────────────────────────────┐
│                  Common (公共层)                       │
│  基础设施、工具函数、统一抽象                           │
│  ┌─────────────────────────────────────────────────┐ │
│  │  io (I/O 抽象)                                   │ │
│  │   - io_context: 统一事件循环                     │ │
│  │   - scheduler: 任务调度                          │ │
│  │   - timer: 定时器封装                           │ │
│  ├─────────────────────────────────────────────────┤ │
│  │  log (日志系统)                                  │ │
│  │   - logger: 基于 io_context 的异步日志           │ │
│  │   - sink: 输出目标 (控制台/文件/网络)           │ │
│  │   - formatter: 日志格式化                        │ │
│  ├─────────────────────────────────────────────────┤ │
│  │  net (网络通信)                                  │ │
│  │   - socket: Socket 封装                         │ │
│  │   - channel: 通信通道                            │ │
│  │   - codec: 编解码器                              │ │
│  ├─────────────────────────────────────────────────┤ │
│  │  ser (序列化)                                    │ │
│  │   - serializer: 序列化接口                       │ │
│  │   - protobuf: protobuf 实现                     │ │
│  │   - json: JSON 实现 (可选)                      │ │
│  ├─────────────────────────────────────────────────┤ │
│  │  event (事件系统)                               │ │
│  │   - event_bus: 事件总线                          │ │
│  │   - event_handler: 事件处理器                    │ │
│  │   - dispatcher: 事件分发                         │ │
│  ├─────────────────────────────────────────────────┤ │
│  │  config (配置管理)                               │ │
│  │   - config_loader: 配置加载器                    │ │
│  │   - config_watcher: 配置热更新                   │ │
│  ├─────────────────────────────────────────────────┤ │
│  │  ecs (实体组件系统)                              │ │
│  │   - registry: 实体注册表                         │ │
│  │   - system: 系统抽象                            │ │
│  │   - view: 视图                                  │ │
│  ├─────────────────────────────────────────────────┤ │
│  │  util (工具函数)                                 │ │
│  │   - string: 字符串工具                          │ │
│  │   - time: 时间工具                              │ │
│  │   - error: 错误处理                             │ │
│  │   - container: 容器工具                         │ │
│  └─────────────────────────────────────────────────┘ │
│  依赖: fmt, boost, entt, protobuf          │
└────────────────┬────────────────────────────────────┘
                 │
┌────────────────▼─────────────────────────────────────┐
│                3rdparty (第三方库)                     │
│  fmt, boost, entt, protobuf, catch           │
└─────────────────────────────────────────────────────┘
```

## 二、目录结构规划

```
src/
├── common/                          # 动态库 - 基础设施层
│   ├── include/common/
│   │   ├── banner.h               # 启动横幅
│   │   ├── define.h               # 公共定义和宏
│   │   ├── git_revision.h         # Git 版本信息
│   │   ├── io/                    # I/O 抽象
│   │   │   ├── io_context.h      # 统一事件循环
│   │   │   ├── scheduler.h       # 任务调度
│   │   │   └── timer.h           # 定时器
│   │   ├── log/                   # 日志系统
│   │   │   ├── logger.h          # 日志接口
│   │   │   ├── sink.h            # 输出目标
│   │   │   ├── console_sink.h    # 控制台输出
│   │   │   ├── file_sink.h       # 文件输出
│   │   │   └── formatter.h       # 格式化器
│   │   ├── net/                   # 网络通信
│   │   │   ├── socket.h
│   │   │   ├── channel.h
│   │   │   └── codec.h
│   │   ├── ser/                   # 序列化
│   │   │   ├── serializer.h      # 序列化接口
│   │   │   ├── protobuf_ser.h    # protobuf 实现
│   │   │   └── json_ser.h        # JSON 实现
│   │   ├── event/                 # 事件系统
│   │   │   ├── event.h           # 事件定义
│   │   │   ├── event_bus.h       # 事件总线
│   │   │   └── handler.h
│   │   ├── config/                # 配置管理
│   │   │   ├── config.h
│   │   │   ├── loader.h
│   │   │   └── watcher.h
│   │   ├── ecs/                   # ECS 系统
│   │   │   ├── registry.h
│   │   │   ├── system.h
│   │   │   └── view.h
│   │   ├── util/                  # 工具函数
│   │   │   ├── string.h
│   │   │   ├── time.h
│   │   │   ├── error.h
│   │   │   └── container.h
│   │   └── common.h              # 公共头文件
│   └── src/
│       ├── io/
│       ├── log/
│       ├── net/
│       ├── ser/
│       ├── event/
│       ├── config/
│       ├── ecs/
│       └── util/
│
├── core/                            # 动态库 - 核心业务层
│   ├── include/core/
│   │   ├── engine/                # 引擎核心
│   │   │   ├── engine.h          # 主引擎类
│   │   │   └── context.h         # 引擎上下文
│   │   ├── loop/                  # 主循环
│   │   │   └── loop.h            # 循环抽象
│   │   ├── state/                 # 状态管理
│   │   │   ├── state_machine.h   # 状态机
│   │   │   └── state.h
│   │   ├── lifecycle/             # 生命周期
│   │   │   └── lifecycle.h
│   │   ├── actor/                 # Actor 系统
│   │   │   ├── actor.h           # Actor 抽象
│   │   │   ├── actor_system.h    # Actor 系统
│   │   │   ├── actor_ref.h       # Actor 引用
│   │   │   ├── message.h         # 消息定义
│   │   │   └── message_queue.h  # 消息队列
│   │   └── core.h
│   └── src/
│       ├── engine/
│       ├── loop/
│       ├── state/
│       ├── lifecycle/
│       └── actor/
│
├── apps/                            # 应用层
│   ├── example/                    # 示例程序
│   ├── server/                     # 服务器
│   └── client/                     # 客户端
└── tests/                           # 单元测试层
    ├── common/                      # Common 层测试
    │   ├── util/
    │   ├── io/
    │   ├── log/
    │   ├── net/
    │   ├── ser/
    │   ├── event/
    │   ├── config/
    │   └── ecs/
    └── core/                       # Core 层测试
        ├── engine/
        ├── loop/
        ├── state/
        ├── lifecycle/
        └── actor/
```

## 三、核心设计理念

### 1. 统一 I/O 模型
- 所有异步操作基于统一的 `io_context`
- 网络和日志共享同一事件循环
- 简化线程管理和资源调度

### 2. 模块化设计
- 每个子模块独立，职责清晰
- 通过接口抽象实现可替换
- 降低模块间耦合

### 3. 零拷贝理念
- 网络传输、日志输出、序列化均考虑零拷贝
- 使用 string_view、buffer 等机制

### 4. 异步优先
- I/O 操作默认异步
- 同步接口作为异步的简化封装

## 四、CMakeLists.txt 配置

### 1. src/common/CMakeLists.txt
```cmake
# 动态库
rendu_add_library(
    DIR ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT ${PROJECT_NAME}
    NAME common
    SHARED
    PRIVATE_LINK rendu-core-interface
    PUBLIC_LINK
        3rdparty::fmt
        3rdparty::boost
        3rdparty::entt

        3rdparty::protobuf
    ALIAS ON
)
```

### 2. src/core/CMakeLists.txt
```cmake
# 动态库
rendu_add_library(
    DIR ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT ${PROJECT_NAME}
    NAME core
    SHARED
    PRIVATE_LINK rendu-core-interface
    PUBLIC_LINK RenduCore::common
    ALIAS ON
)
```

## 五、各层职责

| 层级 | 职责 |
|------|------|
| **Apps** | 业务逻辑、具体应用实现 |
| **Tests** | 单元测试、集成测试、性能测试 |
| **Core** | 框架核心、引擎、生命周期、状态管理、Actor 系统 |
| **Common** | 基础设施、I/O、日志、网络、序列化、事件、配置、ECS、工具 |
| **3rdparty** | fmt, boost, entt, protobuf, catch |
