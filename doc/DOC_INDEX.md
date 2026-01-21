# RenduCore 文档索引

## 架构与规划

- **[架构设计](doc_architecture.md)** - 整体架构和目录结构
- **[开发路线图](doc_roadmap.md)** - 分阶段开发计划
- **[测试结构](doc_tests_structure.md)** - 单元测试组织

---

## 阶段 0: 环境准备 ✅

- **[阶段 0: 环境准备](doc_phase0_env.md)** - CMake 配置、第三方库验证、动态库配置

---

## Common 层文档 (阶段 1-8)

- **[阶段 1: util](doc_phase1_util.md)** - 基础工具 (define/string/time/error/container)
- **[阶段 2: io](doc_phase2_io.md)** - I/O 抽象 (io_context/scheduler/timer)
- **[阶段 3: log](doc_phase3_log.md)** - 日志系统 (logger/sink/formatter)
- **[阶段 4: net](doc_phase4_net.md)** - 网络通信 (socket/channel/codec)
- **[阶段 5: ser](doc_phase5_ser.md)** - 序列化 (serializer/protobuf/json)
- **[阶段 6: event](doc_phase6_event.md)** - 事件系统 (event/event_bus/handler)
- **[阶段 7: config](doc_phase7_config.md)** - 配置管理 (config/loader/watcher)
- **[阶段 8: ecs](doc_phase8_ecs.md)** - ECS 系统 (registry/system/view)

---

## Core 层文档 (阶段 9-12.5)

- **[阶段 9: engine](doc_phase9_engine.md)** - 引擎核心 (engine/context)
- **[阶段 10: loop](doc_phase10_loop.md)** - 主循环 (fixed_loop/variable_loop)
- **[阶段 11: state](doc_phase11_state.md)** - 状态管理 (state_machine/state)
- **[阶段 12: lifecycle](doc_phase12_lifecycle.md)** - 生命周期 (lifecycle/manager)
- **[阶段 12.5: actor](doc_phase12_5_actor.md)** - Actor 系统 (actor/actor_system/actor_ref/message)

---

## Apps 层文档 (阶段 13-14)

- **[阶段 13: example](doc_phase13_example.md)** - 示例程序
- **[阶段 14: server](doc_phase14_server.md)** - 服务器实现

---

## 开发顺序

```
阶段 0: 环境准备 ✅
    ↓
阶段 1-8: Common 层 (从底向上)
    ↓
阶段 9-12.5: Core 层
    ↓
阶段 13-14: Apps 层
```
