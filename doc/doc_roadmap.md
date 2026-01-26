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

## 阶段 9: Core 层 - 引擎核心 (engine)

### 目标
- 实现主引擎类
- 管理引擎生命周期

### 任务
- [ ] `engine.h` - 引擎主类
- [ ] `context.h` - 引擎上下文，管理全局状态

### 验收标准
- 引擎可正常启动/停止
- 资源正确释放

### 依赖
- 阶段 2 (io)
- 阶段 3 (log)
- 阶段 6 (event)

---

## 阶段 10: Core 层 - 主循环 (loop)

### 目标
- 实现主循环抽象
- 支持可插拔的循环策略

### 任务
- [ ] `loop.h` - 循环抽象基类
- [ ] 固定步长循环实现
- [ ] 可变步长循环实现 (可选)

### 验收标准
- 循环可正常启动/停止
- 帧率稳定

### 依赖
- 阶段 9 (engine)
- 阶段 2 (io)

---

## 阶段 11: Core 层 - 状态管理 (state)

### 目标
- 实现状态机
- 管理运行时状态

### 任务
- [ ] `state_machine.h` - 状态机
- [ ] `state.h` - 状态基类

### 验收标准
- 状态转换正确
- 支持状态进入/退出回调

### 依赖
- 阶段 6 (event)

---

## 阶段 12: Core 层 - 生命周期 (lifecycle)

### 目标
- 定义组件生命周期
- 实现统一的初始化/销毁流程

### 任务
- [ ] `lifecycle.h` - 生命周期接口定义
- [ ] 生命周期管理器

### 验收标准
- 组件按正确顺序初始化/销毁
- 无资源泄漏

### 依赖
- 阶段 9 (engine)

---

## 阶段 12.5: Core 层 - Actor 系统 (actor)

### 目标
- 实现轻量级 Actor 模型
- 提供高并发、消息驱动的并发编程能力

### 任务
- [ ] `actor.h` - Actor 抽象基类
- [ ] `actor_system.h` - Actor 系统管理
- [ ] `actor_ref.h` - Actor 引用，支持跨进程通信
- [ ] `message.h` - 消息定义
- [ ] `message_queue.h` - 消息队列，支持优先级和限流
- [ ] `actor_pool.h` (可选) - Actor 对象池

### 验收标准
- Actor 创建/销毁正常
- 消息传递正确（Tell/Ask）
- 多线程安全
- ActorRef 可序列化
- 性能满足预期

### 依赖
- 阶段 2 (io) - io_context 调度
- 阶段 3 (log) - 日志记录
- 阶段 5 (ser) - 消息序列化
- 阶段 6 (event) - 事件通知
- 阶段 12 (lifecycle) - 生命周期管理

---

## 阶段 13: 应用层 - 示例程序 (example)

### 目标
- 演示框架使用方法
- 验证各模块集成

### 任务
- [ ] 创建示例项目
- [ ] 日志示例
- [ ] 网络示例
- [ ] 事件示例
- [ ] ECS 示例
- [ ] Actor 示例
- [ ] 引擎示例

### 验收标准
- 示例可正常编译运行
- 文档清晰

### 依赖
- 阶段 12.5 (actor) - Actor 系统

---

## 阶段 14: 应用层 - 服务器 (server)

### 目标
- 基于框架实现一个完整的服务器示例
- 验证框架在生产环境下的表现

### 任务
- [ ] 网络服务器实现
- [ ] 协议处理
- [ ] Actor 管理层
- [ ] 日志记录
- [ ] 性能优化

### 验收标准
- 服务器稳定运行
- 性能满足预期

### 依赖
- 阶段 12.5 (actor) - Actor 系统
