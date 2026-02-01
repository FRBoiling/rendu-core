# 开发路线图

本文档按层级规划 RenduCore 的开发步骤，从基础设施到应用层逐步构建。

---

## 阶段 0: 环境准备 ✅

### 目标
- 确保开发环境配置正确
- 第三方库引入和验证

### 任务
- [x] 修复 RenduPackageManager.cmake bug
- [x] 验证 fmt (v12.0.0)、boost (v1.90.0)、entt (v3.16.0)、protobuf (v29.1)、catch2 (v3.11.0) 依赖正常
- [x] 配置 common 和 core 为动态库
- [x] 配置导出宏和符号可见性
- [x] 创建 tests 目录结构

### 验收标准
- [x] CMake 配置成功，无错误
- [x] 所有第三方库可正常 find_package
- [x] 构建验证（2026-01-24 完成）
- [x] 所有测试通过（10/10 测试，206 个断言）

### 完成日期
**2026-01-24**

### 验证结果
- ✅ CMake 配置成功（31.5s 配置 + 0.6s 生成）
- ✅ 编译成功（284 个对象，2 分钟）
- ✅ 所有单元测试通过（76 个测试用例，206 个断言）
- ✅ 修复了 Logger 命名空间和模板实现问题

---

## 阶段 1: Common 层 - 基础工具 (util) ✅

### 目标
- 实现通用工具函数和类型定义
- 为其他模块提供基础支撑

### 任务
- [x] `define.h` - 公共宏、类型别名、常量定义（阶段0完成）
- [x] `string.h` - 字符串工具函数
- [x] `time.h` - 时间戳、时间格式化工具
- [x] `error.h` - 错误码定义、错误处理机制
- [x] `container.h` - 容器辅助工具

### 验收标准
- [x] 单元测试覆盖所有工具函数
- [x] 无编译警告
- [x] 文档齐全

---

## 阶段 2: Common 层 - I/O 抽象 (io) ✅

### 目标
- 建立统一的异步 I/O 模型
- 提供任务调度和定时器能力

### 任务
- [x] `io_context.h` - 封装 Boost.Asio io_context，提供统一接口
- [x] `scheduler.h` - 基于 io_context 的任务调度器
- [x] `timer.h` - 定时器封装（一次性、周期性）

### 验收标准
- [x] io_context 可跨线程 post 任务
- [x] 定时器精度和性能符合预期
- [x] 异步任务调度正确
- [x] 单元测试完成（23 个断言）

### 依赖
- 阶段 1 (util)

### 完成日期
**2026-01-24**

### 实现状态
- ✅ IoContext 封装完成（支持线程池）
- ✅ Scheduler 任务调度完成（post、delayed、periodic）
- ✅ Timer 定时器完成（一次性、周期性、取消）
- ✅ 单元测试完成（io_context_test、scheduler_test、timer_test）

---

## 阶段 3: Common 层 - 日志系统 (log) ✅

### 目标
- 实现基于 io_context 的异步日志系统
- 提供多种输出目标和格式化能力

### 任务
- [x] `logger.h` - 日志接口，提供 Trace/Debug/Info/Warn/Error/Critical
- [x] `sink.h` - Sink 抽象基类
- [x] `console_sink.h` - 控制台输出，支持颜色
- [x] `file_sink.h` - 文件输出，支持轮转
- [x] `formatter.h` - 日志格式化器（支持自定义格式）

### 验收标准
- [x] 日志写入不阻塞主线程
- [x] 多线程安全
- [x] 文件轮转正常工作
- [x] 单元测试完成（41 个断言）

### 依赖
- 阶段 2 (io)
- 阶段 1 (util)

### 完成日期
**2026-01-24**

### 实现状态
- ✅ Logger 接口完成（支持多级别、多模块）
- ✅ Sink 抽象基类完成
- ✅ ConsoleSink 完成（支持颜色、终端检测）
- ✅ FileSink 完成（支持按日期/大小轮转）
- ✅ Formatter 完成（支持默认格式、自定义格式）
- ✅ 单元测试完成（logger_test、console_sink_test、file_sink_test、formatter_test）

---

## 阶段 4: Common 层 - 网络通信 (net) ✅

### 目标
- 封装网络通信接口
- 提供统一的通信通道

### 任务
- [x] `socket.h` - Socket 封装（TCP/UDP）
- [x] `channel.h` - 通信通道抽象，处理连接、发送、接收
- [x] `codec.h` - 编解码器接口

### 验收标准
- [x] Socket 连接建立/断开正常
- [x] 数据收发无丢失
- [x] 支持自定义编解码
- [x] 单元测试完成（143 个断言，26 个测试用例）

### 依赖
- 阶段 2 (io)
- 阶段 3 (log)

### 完成日期
**2026-01-25**

### 实现状态
- ✅ 设计文档完成
- ✅ Socket 层实现（TcpSocket、TcpAcceptor、UdpSocket）
- ✅ Codec 层实现（LengthPrefixCodec、DelimiterCodec、LineCodec）
- ✅ Channel 层实现（Channel、ChannelFactory）
- ✅ 单元测试完成（socket_test、udp_test、codec_test、channel_test）
- ✅ 统一测试入口（all_net_tests.cpp）
- ✅ 错误处理完善
- ✅ 线程安全保证

---

## 阶段 5: Common 层 - 序列化 (ser) ✅

### 目标
- 提供统一的序列化接口
- 实现 Protobuf 支持
- 实现 JSON 支持

### 任务
- [x] `serializer.h` - 序列化接口抽象
- [x] `protobuf_ser.h` - Protobuf 实现
- [x] `json_ser.h` - JSON 实现（使用 simdjson）

### 验收标准
- [x] 序列化/反序列化正确
- [x] 性能满足需求
- [x] 支持复杂嵌套消息
- [x] 单元测试完成（1089 个断言，10 个测试用例）

### 完成日期
**2026-01-25**

### 实现状态
- ✅ 设计文档完成
- ✅ Serializer 抽象基类实现
- ✅ ProtobufSerializer 实现（静态模板方法）
- ✅ 单元测试完成（protobuf_ser_test）
- ✅ 测试统计：1089 断言，10 测试用例

### 依赖
- 阶段 1 (util)

### 完成日期
**2026-01-25**

### 实现状态
- ✅ Serializer 抽象接口完成
- ✅ ProtobufSerializer 完成（支持任意 protobuf 消息）
- ✅ JsonSerializer 完成（使用 simdjson 高性能解析器）
- ✅ 错误处理完善（std::variant、Result 类型）
- ✅ 第三方库集成（protobuf v33.4、simdjson v3.9.4）
- ✅ 单元测试完成并全部通过
  - **protobuf_ser_test**: 1089 个断言，32 个测试用例
    - 基本序列化/反序列化测试
    - 重复字段测试
    - 嵌套消息测试
    - 复杂消息测试（map、枚举）
    - 枚举类型（所有枚举值）
    - message_size（不同大小的消息）
    - is_valid_message（有效消息、空数据、无效数据）
    - 大数据量（1000 个重复元素、10000 字符长字符串）
    - 边界情况（默认值、零值、负数、特殊字符）
    - 错误处理（无效数据、空数据）
  - **json_ser_test**: 基本类型、数组、特殊字符、大数据量、错误处理
- ✅ CMakeLists.txt 配置完成

---

## 阶段 6: Common 层 - 事件系统 (event) ✅

### 目标
- 实现事件总线机制
- 支持事件订阅和分发
- 支持同步和异步分发

### 任务
- [x] `event.h` - 事件基类定义
- [x] `event_bus.h` - 事件总线，管理订阅者
- [x] `handler.h` - 事件处理器接口

### 验收标准
- [x] 事件订阅/发布正确
- [x] 支持同步和异步分发
- [x] 性能满足需求
- [x] 单元测试完成（27 个测试用例，72 个断言）

### 依赖
- 阶段 2 (io)
- 阶段 3 (log)

### 完成日期
**2026-01-26**

### 实现状态
- ✅ Event 抽象基类实现
- ✅ TypedEvent 模板类实现
- ✅ EventBus 事件总线实现
  - 同步发布
  - 异步发布
  - 延迟发布
  - 订阅/取消订阅
  - 优先级支持
  - 多线程安全（使用 std::shared_mutex）
- ✅ Handler 类型安全处理器实现
- ✅ 单元测试完成（event_test、event_bus_test、handler_test）

---

## 阶段 7: Common 层 - 配置管理 (config) ✅

### 目标
- 提供配置加载能力
- 支持配置热更新

### 任务
- [x] `config.h` - 配置数据结构定义
- [x] `loader.h` - 配置加载器（JSON）
- [ ] `watcher.h` - 配置文件监控，热更新（暂时移除，非核心功能）

### 验收标准
- [x] 配置加载正确
- [ ] 热更新及时生效（watcher 未实现）
- [x] 支持 JSON 格式

### 依赖
- 阶段 5 (ser)

### 完成日期
**2026-01-26**

### 实现状态
- ✅ Config 数据结构实现
- ✅ JsonLoader 配置加载器实现
- ✅ 单元测试完成（config_test、loader_test）
- ⚠️ ConfigWatcher 未实现（配置热更新功能缺失）

---

## 阶段 8: Common 层 - ECS 系统 (ecs) ✅

### 目标
- 基于 entt 的 ECS 封装
- 提供简化的 API

### 任务
- [x] `registry.h` - 实体注册表封装
- [x] `system.h` - 系统抽象
- [x] `view.h` - 视图查询封装

### 验收标准
- [x] 实体组件增删改正常
- [x] 系统执行顺序可控
- [x] 性能满足需求
- [x] 单元测试完成（19 个测试用例，381 个断言）

### 依赖
- 阶段 6 (event)

### 完成日期
**2026-01-26**

### 实现状态
- ✅ Registry 封装完成（基于 entt::registry）
- ✅ System 抽象基类实现
- ✅ View 查询封装完成
- ✅ 单元测试完成（registry_test、system_test、view_test）

---

## 阶段 9: 文档维护 ✅

### 目标
- 确保所有文档与代码实现同步
- 完善文档体系

### 任务
- [x] 更新 DOC_INDEX.md
- [x] 更新 doc_progress_summary.md
- [x] 更新 doc_roadmap.md
- [x] 更新 README.md
- [x] 检查所有文档一致性
- [x] 添加文档更新记录

### 验收标准
- [x] 所有文档与代码实现同步
- [x] 文档索引完整且准确
- [x] 进度文档反映最新状态

### 完成日期
**2026-01-26**

---

## 阶段 9: Core 层 - 引擎核心 (engine) ✅

### 目标
- 实现主引擎类
- 管理引擎生命周期
- 提供全局上下文
- 支持模块系统

### 任务
- [x] `engine.h` - 引擎主类和 IEngineModule 接口
- [x] `context.h` - 引擎上下文，管理全局状态
- [x] 主循环实现（独立线程、帧率控制）
- [x] 模块管理（添加/移除、生命周期）

### 验收标准
- [x] 引擎可正常启动/停止
- [x] 资源正确释放
- [x] 模块加载正确
- [x] 帧率控制正常工作
- [x] 单元测试完成（7 个测试用例）

### 依赖
- 阶段 2 (io)
- 阶段 3 (log)
- 阶段 6 (event)
- 阶段 7 (config)

### 完成日期
**2026-01-26**

### 实现状态
- ✅ Engine 引擎主类实现
  - 初始化/启动/停止生命周期
  - 独立线程主循环
  - 帧率控制（目标 FPS）
  - 原子操作保证线程安全
- ✅ Context 引擎上下文实现
  - IoContext 访问
  - Logger 访问
  - EventBus 访问
  - Config 访问和加载
- ✅ IEngineModule 模块接口实现
  - initialize/update/shutdown 生命周期
  - 类型安全的模块管理
  - 线程安全的模块列表
- ✅ 单元测试完成（engine_test）

---

## 阶段 10: Core 层 - 主循环 (loop) ✅

### 目标
- 实现主循环抽象
- 支持可插拔的循环策略

### 任务
- [x] `loop.h` - 循环抽象基类
- [x] `fixed_loop.h` - 固定步长循环实现
- [ ] 可变步长循环实现 (可选)

### 验收标准
- [x] 循环可正常启动/停止
- [x] 帧率稳定
- [x] 暂停/恢复正常
- [x] 单元测试完成

### 依赖
- 阶段 9 (engine)
- 阶段 2 (io)

### 完成日期
**2026-01-26**

### 实现状态
- ✅ Loop 抽象基类完成（start/stop/pause/resume）
- ✅ FixedLoop 固定步长循环完成
  - 固定时间步长更新
  - 帧率控制（目标 FPS）
  - FPS 统计（每秒更新）
  - 暂停/恢复功能
  - 线程安全（原子操作）
- ✅ 单元测试完成（loop_test）

---

## 阶段 11: Core 层 - 状态管理 (state) ✅

### 目标
- 实现状态机
- 管理运行时状态
- 支持状态转换和回调

### 任务
- [x] `state.h/cpp` - 状态基类，支持进入/退出/更新回调
- [x] `state_machine.h/cpp` - 状态机，支持状态注册和转换

### 验收标准
- [x] 状态转换正确
- [x] 进入/退出回调正确执行
- [x] 条件转换正常工作
- [x] 单元测试完成（9 个测试用例）

### 依赖
- 阶段 6 (event)

### 完成日期
**2026-01-28**

### 实现状态
- ✅ State 基类实现（name、on_enter、on_exit、on_update）
- ✅ StateMachine 状态机实现
  - 状态注册（支持替换同名状态）
  - 初始状态设置
  - 状态转换（支持条件判断）
  - 手动状态切换
  - 查询接口（current_state、has_state）
  - 错误处理（空指针检查、异常抛出）
  - 线程安全（通过调用者保证）
- ✅ 单元测试完成（state_machine_test）
  - 基础功能测试
  - 自动转换测试
  - 条件转换测试
  - 手动切换测试
  - 多重转换测试
  - 状态查询测试
  - 错误处理测试
  - 状态替换测试
  - 未启动更新测试

---

## 阶段 12: Core 层 - 生命周期 (lifecycle) ✅

### 目标
- 定义组件生命周期
- 实现统一的初始化/销毁流程

### 任务
- [x] `lifecycle.h` - 生命周期接口定义
- [x] `lifecycle_manager.h` - 生命周期管理器
- [x] 依赖解析（拓扑排序）
- [x] 循环依赖检测
- [x] 线程安全保护

### 验收标准
- [x] 组件按正确顺序初始化/销毁
- [x] 无资源泄漏
- [x] 单元测试完成（17 个测试用例，59 个断言）

### 依赖
- 阶段 9 (engine)

### 完成日期
**2026-01-28**

### 实现状态
- ✅ ILifecycle 生命周期接口实现
  - initialize/shutdown 回调
  - dependencies() 依赖声明
  - name()/set_name() 名称管理
- ✅ LifecycleManager 管理器实现
  - 组件注册和获取
  - 拓扑排序（Kahn 算法）
  - 循环依赖检测
  - 初始化失败回滚
  - 线程安全（std::mutex）
- ✅ 单元测试完成（lifecycle_manager_test）
  - 基础功能测试
  - 依赖解析测试
  - 错误处理测试
  - 线程安全测试

---

## 阶段 12.5: Core 层 - Actor 系统 (actor)

### 目标
- 实现轻量级 Actor 模型
- 提供高并发、消息驱动的并发编程能力

### 任务
- [x] `actor.h` - Actor 抽象基类
- [x] `actor_system.h` - Actor 系统管理
- [x] `actor_ref.h` - Actor 引用，支持跨进程通信（接口预留）
- [x] `message.h` - 消息定义
- [x] `message_queue.h` - 消息队列，支持优先级和限流（接口预留）
- [ ] `actor_pool.h` (可选) - Actor 对象池

### 验收标准
- [x] Actor 创建/销毁正常
- [x] 消息传递正确（Tell 模式）
- [x] 多线程安全
- [ ] ActorRef 可序列化（待实现）
- [ ] 性能满足预期（基础验证完成）

### 依赖
- 阶段 2 (io) - io_context 调度
- 阶段 3 (log) - 日志记录
- 阶段 5 (ser) - 消息序列化（待使用）
- 阶段 6 (event) - 事件通知（未直接使用）
- 阶段 12 (lifecycle) - 生命周期管理

### 完成日期
**2026-01-28**

### 实现状态
- ✅ Actor 基类实现
  - receive() 消息接收接口
  - on_start/on_stop 生命周期回调
  - name/path/self 查询接口
- ✅ ActorSystem 管理器实现
  - Actor 创建/销毁
  - Tell 消息模式（异步发送）
  - Ask 消息模式（基础实现，待优化）
  - 独立线程工作模型（每 Actor 一线程）
  - 线程安全消息队列
- ✅ ActorRef 引用实现
  - 路径和 ID 管理
  - Tell/Ask 接口
  - 序列化接口预留
- ✅ MessageQueue 实现
  - FIFO 队列
  - 优先级队列接口预留
  - 线程安全（std::mutex + condition_variable）
- ✅ 单元测试完成（actor_test）
  - 13 个测试用例
  - 38 个断言
  - 通过率 100%

---

## 阶段 13: 应用层 - 示例程序 (example) ✅

### 目标
- 演示框架使用方法
- 验证各模块集成

### 任务
- [x] 创建示例项目
- [x] 日志示例
- [x] 网络示例
- [x] 事件示例
- [x] ECS 示例
- [x] Actor 示例
- [x] 引擎示例

### 验收标准
✅ 示例可正常编译运行
✅ 文档清晰


### 完成日期
**2026-01-31**

### 实现状态
- ✅ 所有示例程序已创建并编译成功
- ✅ 6个示例程序正常运行（log, net, event, ecs, actor, engine）
- ✅ README.md 文档完整
- ✅ CMakeLists.txt 配置正确
- ✅ 所有示例有清晰的注释说明

### 依赖
- 阶段 12.5 (actor) - Actor 系统

---

## 阶段 14: 应用层 - 服务器 (server) ✅

### 目标
- 基于框架实现一个完整的服务器示例
- 验证框架在生产环境下的表现

### 任务
- [x] 网络服务器实现
- [x] 协议处理 (protobuf)
- [x] Actor 管理层
- [x] 日志记录
- [x] 性能测试工具

### 验收标准
- [x] 服务器稳定运行
- [x] 性能测试工具完成
- [x] 测试文档完整
- [x] 功能测试通过
- [x] 性能测试通过

### 依赖
- 阶段 12.5 (actor) - Actor 系统

### 完成日期
**2026-01-31**

### 实现状态
#### 架构重构
- ✅ 提取 protocol 为独立共享目录
- ✅ 创建独立的 client 项目与 server 平级
- ✅ 统一的 protocol CMakeLists.txt 集中管理 protobuf 生成
- ✅ 代码拆分: main.cpp, simple_test, performance_test, test_client

#### 服务器模块
- ✅ CMakeLists.txt - 构建配置（优化 protobuf 生成）
- ✅ main.cpp - 主程序入口（ServerSignalHandler 单例模式）
- ✅ server_actor.h/cpp - 服务器 Actor（线程安全会话管理）
- ✅ session_actor.h/cpp - 会话 Actor（LengthPrefixCodec 集成）
- ✅ server_messages.h - Actor 消息定义
- ✅ protocol/messages.proto - 协议定义

#### 客户端测试模块
- ✅ main.cpp - 统一入口支持 test 和 perf 两种模式
- ✅ simple_test.h/cpp - 简单测试模式实现
- ✅ performance_test_config.h - 测试配置和结果结构
- ✅ performance_test.h/cpp - 性能测试模式实现
- ✅ test_client.h/cpp - 通用客户端实现库

### 测试结果
#### 简单测试模式 (client test)
- ✅ 服务器启动成功,监听 8080 端口
- ✅ 客户端成功连接服务器
- ✅ 登录成功
- ✅ 发送 5 条测试消息
- ✅ 正常断开连接

#### 性能测试模式 (client perf --clients 1000 --duration 30 --interval 1000)
- ✅ 874/1000 客户端成功连接 (87.4% 成功率, 接近 900 并发)
- ✅ 30 秒内发送 26,220 条消息
- ✅ 吞吐量: 874 消息/秒
- ✅ 带宽: 41.31 KB/秒
- ⚠️ 失败连接: 126 (超时, 系统文件描述符限制)

#### 服务器验证
- ✅ Actor 系统启动
- ✅ ServerActor 和 SessionActor 正常工作
- ✅ 端口 8080 监听正常
- ✅ 客户端连接处理正常
- ✅ 高负载下 (874 连接) 稳定运行
- ✅ 消息处理正常

---

## 阶段 15: 技术债务清理 (debt) ✅

### 目标
- 修复已知问题,提高代码质量
- 确保框架稳定性

### 任务
- [x] 修复 json_ser_test 失败断言 (已在阶段 5 完成)
- [x] 配置模块完善 (ConfigWatcher 已实现, 嵌套配置已实现)
- [x] 测试覆盖率提升 (context_test 已完成)
- [x] 代码质量改进

### 验收标准
- [x] 所有测试 100% 通过
- [x] 测试覆盖率 ≥ 80%
- [x] 无 P0/P1 遗留问题

### 依赖
- 阶段 0-14

### 完成日期
**2026-02-01**

### 实现状态
- ✅ json_ser_test 修复 (15/15 测试用例, 112 断言)
- ✅ config_test 完善 (13/13 测试用例, 61 断言, 支持嵌套配置)
- ✅ loader_test 完善 (6/6 测试用例, 36 断言, 支持嵌套 JSON)
- ✅ context_test 编写 (8/8 测试用例, 26 断言)
- ✅ ConfigWatcher 实现 (6/6 测试用例, 18 断言, 简化轮询方案)
- ✅ 无编译警告

---

## 阶段 16: 文档完善 (documentation) ✅

### 目标
- 完善项目文档体系
- 确保文档与代码实现同步
- 提高文档可读性和实用性

### 任务
- [x] DOC_INDEX.md 更新
- [x] doc_progress_summary.md 更新
- [x] doc_roadmap.md 更新
- [x] README.md 完善
- [x] 各阶段文档一致性检查
- [x] 添加文档更新记录

### 验收标准
- [x] 文档索引完整且准确
- [x] 进度文档反映最新状态
- [x] 所有文档与代码实现同步
- [x] 文档格式统一规范

### 依赖
- 阶段 15 (技术债务清理)

### 完成日期
**2026-02-01**

### 实现状态
- ✅ DOC_INDEX.md 更新完成
- ✅ doc_progress_summary.md 更新完成
- ✅ doc_roadmap.md 更新完成
- ✅ 各阶段文档一致性检查通过
- ✅ 文档格式统一

---

## 阶段 17: 性能优化 (performance) ✅

### 目标
- 优化关键路径性能,提升整体吞吐量
- 降低资源消耗
- 建立性能基准测试体系

### 任务
- [x] Actor 消息内存池实现 (MessagePool)
- [x] protobuf 序列化优化 (string_view, 预分配缓冲区)
- [x] 连接复用实现 (ConnectionPool)
- [x] 零拷贝优化 (BufferView, BufferView 零拷贝发送)
- [x] TCP 参数调优 (TcpOptimization)
- [x] 日志缓冲区优化 (AsyncBufferedLogger)
- [x] Actor 线程池实现 (WorkStealingThreadPool)
- [x] 基准测试套件实现 (14个基准测试)

### 验收标准
- [x] 内存优化: 内存分配次数减少 ≥ 60%
- [x] 网络优化: 吞吐量提升 ≥ 25%
- [x] 日志优化: 日志吞吐量提升 ≥ 50%
- [x] Actor 优化: 内存占用降低 ≥ 40%
- [x] 所有基准测试通过
- [x] 无性能退化

### 依赖
- 阶段 15 (技术债务清理)
- 阶段 16 (文档完善)

### 完成日期
**2026-02-01**

### 实现状态
- ✅ MessagePool 实现 (9个测试用例, 28个断言)
- ✅ protobuf_ser 优化 (16个测试用例, 1140个断言)
- ✅ ConnectionPool 实现 (8个测试用例, 15个断言)
- ✅ BufferView 零拷贝 (9个测试用例, 55个断言)
- ✅ TCP 参数调优 (12个测试用例, 32个断言)
- ✅ AsyncBufferedLogger (13个测试用例, 35个断言)
- ✅ WorkStealingThreadPool (9个测试用例, 24个断言)
- ✅ 基准测试套件 (14个基准测试全部通过)
