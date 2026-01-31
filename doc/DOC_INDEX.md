# RenduCore 文档索引

## 架构与规划

||- **[架构设计](doc_architecture.md)** - 整体架构和目录结构
||- **[开发路线图](doc_roadmap.md)** - 分阶段开发计划
||- **[测试结构](doc_tests_structure.md)** - 单元测试组织
||- **[构建优化指南](build_optimization.md)** - 避免重复下载依赖的配置方法

---

## 阶段 0: 环境准备 ✅

||- **[阶段 0: 环境准备](doc_phase0_env.md)** - CMake 配置、第三方库验证、动态库配置

---

## 阶段 1: Common 层 - 基础工具 (util) ✅

||- **[阶段 1: util](doc_phase1_util.md)** - 基础工具 (define/string/time/error/container)

---

## 阶段 2: Common 层 - I/O 抽象 (io) ✅

||- **[阶段 2: io](doc_phase2_io.md)** - I/O 抽象 (io_context/scheduler/timer)

---

## Common 层文档 (阶段 3-8)

||- **[阶段 3: log](doc_phase3_log.md)** - 日志系统 (logger/sink/formatter)
||- **[阶段 4: net](doc_phase4_net.md)** - 网络通信 (socket/channel/codec)
||- **[阶段 5: ser](doc_phase5_ser.md)** - 序列化 (serializer/protobuf/json)
||- **[阶段 6: event](doc_phase6_event.md)** - 事件系统 (event/event_bus/handler)
||- **[阶段 7: config](doc_phase7_config.md)** - 配置管理 (config/loader/watcher)
||- **[阶段 8: ecs](doc_phase8_ecs.md)** - ECS 系统 (registry/system/view)

---

## Core 层文档 (阶段 9-12.5)

||- **[阶段 9: engine](doc_phase9_engine.md)** - 引擎核心 (engine/context) ✅
||- **[阶段 10: loop](doc_phase10_loop.md)** - 主循环 (loop/fixed_loop) ✅
||- **[阶段 11: state](doc_phase11_state.md)** - 状态管理 (state_machine/state) ✅
||- **[阶段 12: lifecycle](doc_phase12_lifecycle.md)** - 生命周期 (lifecycle/manager) ✅
||- **[阶段 12.5: actor](doc_phase12_5_actor.md)** - Actor 系统 (actor/actor_system/actor_ref/message) ✅

---

## Apps 层文档 (阶段 13-14)

||- **[阶段 13: example](doc_phase13_example.md)** - 示例程序 ✅
||- **[阶段 14: server](doc_phase14_server.md)** - 服务器实现 ✅

---

## 未来阶段文档 (阶段 15-19)

||- **[阶段 15: debt](doc_phase15_debt.md)** - 技术债务清理 ⏳
||- **[阶段 16: documentation](doc_phase16_documentation.md)** - 文档完善 ⏳
||- **[阶段 17: performance](doc_phase17_performance.md)** - 性能优化 ⏳
||- **[阶段 18: advanced](doc_phase18_advanced.md)** - 高级特性 ⏳
||- **[阶段 19: production](doc_phase19_production.md)** - 生产环境 ⏳

---

## 开发顺序

```
阶段 0: 环境准备 ✅
    ↓
阶段 1: Common 层 - util ✅
    ↓
阶段 2: Common 层 - io ✅
    ↓
阶段 3: Common 层 - log ✅
    ↓
阶段 4: Common 层 - net ✅
    ↓
阶段 5: Common 层 - ser ✅
    ↓
阶段 6: Common 层 - event ✅
    ↓
阶段 7: Common 层 - config ✅
    ↓
阶段 8: Common 层 - ecs ✅
    ↓
Common 层完成 ✓
    ↓
阶段 9: Core 层 - engine ✅
    ↓
阶段 10: Core 层 - loop ✅
    ↓
阶段 11: Core 层 - state ✅
    ↓
阶段 12: Core 层 - lifecycle ✅
    ↓
阶段 12.5: Core 层 - actor ✅
    ↓
阶段 13: Apps 层 - example ✅
    ↓
阶段 14: Apps 层 - server ✅
    ↓
Apps 层完成 ✓
    ↓
阶段 15: 技术债务清理 (可选)
    ↓
阶段 16-19: 优化与生产部署
```

---

**最后更新**: 2026-01-31
