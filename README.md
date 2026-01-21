# RenduCore ECS 框架

[![CMake](https://img.shields.io/badge/CMake-3.24+-blue.svg)](https://cmake.org)
[![C++ Standard](https://img.shields.io/badge/C++-20-orange.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

高性能 C++20 ECS（Entity Component System）框架，采用 SOA（Structure of Arrays）内存布局，提供极致的性能和灵活的组件管理。

## ✨ 特性

- 🚀 **极致性能** - SOA 布局，缓存友好，10000 实体遍历仅需 0.13ms
- 🔒 **类型安全** - 模板元编程保证编译时类型检查
- 🧵 **线程安全** - 读写锁策略，支持高并发访问
- 📊 **可调试** - 内置性能分析和内存分析工具
- 🔌 **可扩展** - 支持动态组件、关系管理和事件系统
- 🎯 **易于使用** - 清晰的 API 设计和丰富的示例

## 🚀 快速开始

### 前置要求

- CMake ≥ 3.24
- C++20 兼容编译器（GCC 9+ / Clang 12+ / MSVC 19+）

### 构建项目

```bash
# 配置项目（Debug 模式）
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DRENDU_BUILD_TESTING=ON

# 配置项目（Release 模式）
cmake -B build-release -DCMAKE_BUILD_TYPE=Release

# 编译项目
cmake --build build

# 运行测试
./build/src/apps/tests/RenduCore_tests

# 运行示例
./build/src/apps/example/RenduCore_phase4_demo
```

### 代码示例

```cpp
#include "common/ecs/registry_optimized.h"

using namespace Rendu;

// 定义组件
struct Position {
    float x, y, z;
    Position(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
};

struct Velocity {
    float dx, dy, dz;
    Velocity(float dx = 0, float dy = 0, float dz = 0) : dx(dx), dy(dy), dz(dz) {}
};

int main() {
    RegistryOptimized registry;

    // 创建实体并添加组件
    auto e1 = registry.create();
    registry.emplace<Position>(e1, Position{0, 0, 0});
    registry.emplace<Velocity>(e1, Velocity{1, 0, 0});

    // 查询并更新实体
    auto view = registry.view<Position, Velocity>();
    view.each([](Entity e, Position& pos, Velocity& vel) {
        pos.x += vel.dx;
        pos.y += vel.dy;
        pos.z += vel.dz;
    });

    return 0;
}
```

## 📊 性能基准

| 操作 | RenduCore | EnTT | 优势 |
|------|-----------|------|------|
| 双组件遍历 (10k×10) | 1.48 ms | 19.41 ms | **13.12x** ⚡ |
| 四组件遍历 (10k×10) | 1.69 ms | 34.29 ms | **20.29x** 🚀 |
| 实体创建 (10k) | 10.60 ms | 23.77 ms | **2.24x** |
| L1缓存命中率 | 85% | 45% | **+89%** |

*测试环境：ARM64 (Apple Silicon), Clang 17.0.0, -O2 (Release)*

详见 [性能基准报告](doc/PERFORMANCE.md)

## 📖 文档

完整的文档请查看 [doc/INDEX.md](doc/INDEX.md)

### 快速导航

- 📘 **[使用指南](doc/GUIDE.md)** - 快速上手 ECS 框架
- 📗 **[核心概念](doc/CONCEPTS.md)** - 理解 ECS 架构设计
- 📙 **[性能报告](doc/PERFORMANCE.md)** - 与 EnTT 的性能对比
- 📕 **[缓存优化](doc/CACHE_OPTIMIZATION.md)** - SOA 架构详解
- 📓 **[测试说明](doc/TESTING.md)** - 单元测试指南

### 功能文档

- **[实体管理](doc/FEATURES/ENTITY.md)** - 实体创建、销毁、查询
- **[组件管理](doc/FEATURES/COMPONENT.md)** - 组件添加、移除、访问
- **[查询系统](doc/FEATURES/QUERY.md)** - 视图查询、遍历、过滤
- **[事件系统](doc/FEATURES/EVENT.md)** - 信号与事件机制
- **[关系系统](doc/FEATURES/RELATIONSHIP.md)** - 实体关系与层级管理
- **[系统管理](doc/FEATURES/SYSTEM.md)** - 系统执行器与依赖管理
- **[序列化](doc/FEATURES/SERIALIZATION.md)** - 数据持久化与 JSON 导出
- **[线程安全](doc/FEATURES/THREAD_SAFETY.md)** - 多线程并发支持
- **[调试工具](doc/FEATURES/DEBUGGING.md)** - 实体浏览器、性能分析器

## 🏗️ 项目结构

```
rendu-core/
├── cmake/              # CMake 构建配置
├── doc/                # 文档目录
│   ├── ECS_*.md       # ECS 相关文档
│   └── README.md      # 文档索引
├── src/
│   ├── apps/
│   │   ├── example/   # 示例程序
│   │   ├── tests/     # 单元测试
│   │   └── benchmark/ # 性能测试
│   ├── common/         # ECS 核心实现
│   │   ├── include/common/ecs/
│   │   │   ├── registry_optimized.h
│   │   │   ├── archetype.h
│   │   │   ├── thread_safe_registry.h
│   │   │   ├── profiler.h
│   │   │   ├── archive.h
│   │   │   ├── relationships.h
│   │   │   └── events.h
│   │   └── src/ecs/
│   └── core/          # 核心组件
├── CMakeLists.txt
└── README.md
```

## 🎯 核心功能

### 1. 实体管理
- 创建和销毁实体
- 实体 ID 和版本管理
- 批量创建实体

### 2. 组件管理
- 添加、获取、移除组件
- 动态组件管理
- 类型安全的组件访问

### 3. 查询系统
- 单组件查询
- 多组件查询
- 排除查询

### 4. Archetype 系统
- SOA 内存布局
- 自动实体分组
- 高效数据遍历

### 5. 性能分析
- 操作耗时统计
- 内存使用分析
- 便捷的性能分析宏

### 6. 线程安全
- 读写锁策略
- 读操作共享锁
- 写操作独占锁

### 7. 序列化
- 保存/恢复注册表状态
- JSON 格式支持

### 8. 事件系统
- 实体创建事件
- 实体销毁事件
- 组件更新事件

### 9. 关系系统
- 父子关系管理
- 子实体查询
- 关系遍历

## 🧪 测试

```bash
# 构建测试
cmake --build cmake-build-debug

# 运行所有测试
cd cmake-build-debug
ctest --output-on-failure

# 运行特定测试
./RenduCore_registry_test

# 列出所有测试
./RenduCore_registry_test --list-tests

# 运行特定标签
./RenduCore_registry_test "[registry][basic]"
```

详见 [测试说明](doc/TESTING.md)

## 📝 构建选项

| 选项 | 默认值 | 说明 |
|------|---------|------|
| `RENDU_BUILD_TESTING` | ON | 构建测试 |
| `RENDU_BUILD_BENCHMARK` | OFF | 构建性能测试 |
| `RENDU_BUILD_EXAMPLES` | OFF | 构建示例程序 |
| `CMAKE_BUILD_TYPE` | Debug | 构建类型 (Debug/Release) |

## 🤝 贡献

欢迎贡献！请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解详情。

## 📄 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件。

## 📞 联系方式

- **项目地址**: https://github.com/xxx/rendu-core
- **问题反馈**: https://github.com/xxx/rendu-core/issues
- **讨论区**: https://github.com/xxx/rendu-core/discussions
- **文档中心**: [doc/README.md](doc/README.md)

---

**版本**: 1.2.3
**最后更新**: 2026-01-19
**当前状态**: v1.2.3 - 生产就绪
