# RenduCore

现代化的 C++20 框架，提供基础设施、核心引擎和应用开发能力。

![CMake](https://img.shields.io/badge/CMake-3.24+-blue.svg)
![C++ Standard](https://img.shields.io/badge/C++-20-orange.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

---

## ✨ 特性

- 🏗️ **分层架构** - Apps / Core / Common / 3rdparty 清晰分层
- 🚀 **高性能** - 基于 io_context 的异步 I/O，零拷贝优化
- 🔒 **线程安全** - Actor 模型隔离，事件总线解耦
- 📦 **模块化** - 可插拔的设计，按需引入模块
- 🎯 **易于使用** - 现代化的 C++20 API 设计

---

## 🏗️ 架构

```
┌─────────────────────────────────────────────────────┐
│                   Apps (应用层)                        │
│  示例程序、服务器、客户端、工具                        │
└────────────────┬────────────────────────────────────┘
                 │
┌────────────────▼─────────────────────────────────────┐
│                   Core (核心层)                        │
│  框架核心、引擎、生命周期、状态管理、Actor 系统          │
└────────────────┬────────────────────────────────────┘
                 │
┌────────────────▼─────────────────────────────────────┐
│                  Common (公共层)                       │
│  I/O、日志、网络、序列化、事件、配置、ECS、工具           │
└────────────────┬────────────────────────────────────┘
                 │
┌────────────────▼─────────────────────────────────────┐
│                3rdparty (第三方库)                     │
│  fmt, boost, entt, protobuf, catch           │
└─────────────────────────────────────────────────────┘
```

详见 [架构设计文档](doc/doc_architecture.md)

---

## 🚀 快速开始

### 前置要求

- CMake ≥ 3.24
- C++20 兼容编译器（GCC 9+ / Clang 12+ / MSVC 19+）

### 构建项目

```bash
# 配置项目（Debug 模式）
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug

# 配置项目（Release 模式）
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release

# 编译项目
cmake --build cmake-build-debug

# 运行测试
ctest --test-dir cmake-build-debug

# 运行示例
./cmake-build-debug/src/apps/example/example
```

---

## 📁 项目结构

```
rendu-core/
├── cmake/                  # CMake 构建配置
├── doc/                    # 文档目录
│   ├── doc_architecture.md        # 架构设计
│   ├── doc_roadmap.md           # 开发路线图
│   ├── doc_tests_structure.md    # 测试结构
│   ├── doc_phase*.md           # 各阶段开发文档
│   └── DOC_SUMMARY.md         # 文档索引
├── 3rdparty/               # 第三方库
│   ├── fmt/
│   ├── boost/
│   ├── entt/
│   ├── protobuf/
│   └── catch/
├── src/
│   ├── common/               # Common 层 - 基础设施
│   │   ├── include/common/
│   │   │   ├── io/          # I/O 抽象
│   │   │   ├── log/         # 日志系统
│   │   │   ├── net/         # 网络通信
│   │   │   ├── ser/         # 序列化
│   │   │   ├── event/       # 事件系统
│   │   │   ├── config/       # 配置管理
│   │   │   ├── ecs/         # ECS 系统
│   │   │   ├── util/        # 工具函数
│   │   │   ├── banner.h
│   │   │   ├── define.h
│   │   │   └── git_revision.h
│   │   └── src/
│   ├── core/                 # Core 层 - 核心引擎
│   │   ├── include/core/
│   │   │   ├── engine/      # 引擎核心
│   │   │   ├── loop/        # 主循环
│   │   │   ├── state/       # 状态管理
│   │   │   ├── lifecycle/   # 生命周期
│   │   │   └── actor/       # Actor 系统
│   │   └── src/
│   ├── apps/                 # Apps 层 - 应用程序
│   │   ├── example/          # 示例程序
│   │   ├── server/           # 服务器
│   │   └── client/           # 客户端
│   └── tests/                # Tests 层 - 单元测试
│       ├── common/            # Common 层测试
│       └── core/             # Core 层测试
└── CMakeLists.txt
```

---

## 📚 文档

### 架构与规划

- **[架构设计](doc/doc_architecture.md)** - 整体架构和目录结构
- **[开发路线图](doc/doc_roadmap.md)** - 分阶段开发计划
- **[测试结构](doc/doc_tests_structure.md)** - 单元测试组织

### Common 层文档

- **[阶段 1: util](doc/doc_phase1_util.md)** - 基础工具
- **[阶段 2: io](doc/doc_phase2_io.md)** - I/O 抽象
- **[阶段 3: log](doc/doc_phase3_log.md)** - 日志系统
- **[阶段 4: net](doc/doc_phase4_net.md)** - 网络通信
- **[阶段 5: ser](doc/doc_phase5_ser.md)** - 序列化
- **[阶段 6: event](doc/doc_phase6_event.md)** - 事件系统
- **[阶段 7: config](doc/doc_phase7_config.md)** - 配置管理
- **[阶段 8: ecs](doc/doc_phase8_ecs.md)** - ECS 系统

### Core 层文档

- **[阶段 9: engine](doc/doc_phase9_engine.md)** - 引擎核心
- **[阶段 10: loop](doc/doc_phase10_loop.md)** - 主循环
- **[阶段 11: state](doc/doc_phase11_state.md)** - 状态管理
- **[阶段 12: lifecycle](doc/doc_phase12_lifecycle.md)** - 生命周期
- **[阶段 12.5: actor](doc/doc_phase12_5_actor.md)** - Actor 系统

### Apps 层文档

- **[阶段 13: example](doc/doc_phase13_example.md)** - 示例程序
- **[阶段 14: server](doc/doc_phase14_server.md)** - 服务器

---

## 🧪 测试

```bash
# 运行所有测试
ctest --test-dir cmake-build-debug

# 运行特定测试
./cmake-build-debug/src/tests/common/util/string_test

# 运行特定模块测试
ctest -R util

# 输出详细
ctest -V

# 查看测试覆盖率
cmake --build cmake-build-debug --target coverage
```

测试覆盖率目标：≥ 80%

---

## 📝 开发规范

- C++20 标准
- 遵循 [CODING_STANDARDS.md](cmake/CODING_STANDARDS.md)
- 每个模块需同步编写单元测试
- 使用 Catch2 测试框架

---

## 🤝 贡献

欢迎贡献！请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解详情。

---

## 📄 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件。

---

## 📞 联系方式

- **项目地址**: https://github.com/xxx/rendu-core
- **问题反馈**: https://github.com/xxx/rendu-core/issues
- **讨论区**: https://github.com/xxx/rendu-core/discussions

---

**版本**: v0.1.0-dev
**最后更新**: 2026-01-28
**当前状态**: Common 层已完成，Core 层开发中（阶段 11 已完成）
