# ECS 单元测试

本目录包含使用 Catch2 框架编写的 ECS 系统单元测试。

## 测试文件概览

### 核心功能测试 (5个)
- `registry_test.cpp` - Registry 基础功能测试
- `archetype_test.cpp` - Archetype 存储测试
- `signal_test.cpp` - 信号/观察者系统测试
- `resource_test.cpp` - 资源管理系统测试
- `migration_test.cpp` - 动态组件迁移测试

### 高级功能测试 (4个)
- `relations_test.cpp` - 关系和事件系统测试
- `tools_test.cpp` - 工具系统测试（Profiler, MemoryAnalyzer, CacheAnalyzer等）
- `debugging_test.cpp` - 调试和可视化工具测试
- `performance_test.cpp` - 性能基准测试

### 快速测试 (1个)
- `benchmark_test.cpp` - 快速基准测试

### 可选测试 (1个)
- `profiling_test.cpp` - 深度性能分析测试（运行时间较长）

## 构建测试

### 启用测试

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DRENDU_BUILD_TESTING=ON -B cmake-build-debug
cmake --build cmake-build-debug
```

### 运行所有测试

```bash
cd cmake-build-debug
ctest --output-on-failure
```

### 运行特定测试

```bash
# 运行核心功能测试
./RenduCore_registry_test
./RenduCore_archetype_test
./RenduCore_signal_test
./RenduCore_resource_test

# 运行高级功能测试
./RenduCore_relations_test
./RenduCore_tools_test
./RenduCore_debugging_test

# 运行性能测试
./RenduCore_performance_test
./RenduCore_benchmark_test

# 运行深度分析（可选）
./RenduCore_profiling_test
```

### 运行特定标签的测试

```bash
# 运行所有单元测试
ctest --output-on-failure -L unit

# 运行特定类型的测试
./RenduCore_registry_test "[registry]"
./RenduCore_performance_test "[performance]"
./RenduCore_relations_test "[relations]"
./RenduCore_tools_test "[tools]"
```

### Catch2 选项

```bash
# 列出所有测试用例
./RenduCore_registry_test --list-tests

# 详细输出
./RenduCore_registry_test -s

# 仅运行失败的测试
./RenduCore_registry_test --break

# 基准测试模式
./RenduCore_benchmark_test "[benchmark]"
```

## 测试标签

### 核心标签
- `[registry][basic]` - Registry 基础操作
- `[registry][component]` - 组件管理
- `[registry][view]` - 视图遍历
- `[registry][group]` - 分组操作
- `[registry][snapshot]` - 快照功能
- `[registry][dynamic]` - 动态组件

### Archetype 标签
- `[archetype][basic]` - Archetype 基础
- `[archetype][batch]` - 批量操作
- `[archetype][memory]` - 内存布局
- `[archetype][edge]` - 边界情况

### Signal 标签
- `[signal][construct]` - 构造信号
- `[signal][update]` - 更新信号
- `[signal][destroy]` - 销毁信号
- `[signal][raii]` - RAII 连接管理

### Resource 标签
- `[resource][basic]` - 资源基础操作
- `[resource][exists]` - 资源存在性检查
- `[resource][update]` - 资源更新

### Migration 标签
- `[migration][add]` - 动态添加组件
- `[migration][remove]` - 动态移除组件
- `[migration][update]` - 动态组件更新

### Relations 标签
- `[relations][parent_child]` - 父子关系
- `[relations][query]` - 关系查询
- `[relations][complex]` - 复杂关系结构

### Events 标签
- `[events][pub_sub]` - 发布订阅
- `[events][multiple_types]` - 多种事件类型
- `[events][propagation]` - 事件传播

### Tools 标签
- `[tools][profiler]` - 性能分析器
- `[tools][memory]` - 内存分析器
- `[tools][cache]` - 缓存分析器
- `[tools][hotspot]` - 热点检测器
- `[tools][monitor]` - 性能监控器
- `[tools][integration]` - 工具集成

### Debugging 标签
- `[debugging][browser]` - 实体浏览器
- `[debugging][visualizer]` - 关系可视化
- `[debugging][export]` - 导出功能
- `[debugging][filter]` - 过滤器
- `[debugging][stats]` - 统计信息

### Performance 标签
- `[performance][create]` - 创建性能
- `[performance][view]` - 遍历性能
- `[performance][group]` - 分组性能
- `[performance][dynamic]` - 动态组件性能

### Benchmark 标签
- `[benchmark][create]` - 创建基准测试
- `[benchmark][iterate]` - 遍历基准测试
- `[benchmark][destroy]` - 删除基准测试
- `[benchmark][mixed]` - 混合操作基准测试

## 测试组织说明

### 1. 核心功能测试
这些测试覆盖 ECS 框架的核心功能，是所有测试的基础：
- `registry_test.cpp` - 测试实体管理、组件操作、视图遍历等核心 API
- `archetype_test.cpp` - 测试 Archetype 存储和 SOA 内存布局
- `signal_test.cpp` - 测试信号系统和观察者模式
- `resource_test.cpp` - 测试全局资源管理
- `migration_test.cpp` - 测试动态组件的添加、移除和更新

### 2. 高级功能测试
这些测试覆盖框架的高级特性：
- `relations_test.cpp` - 测试实体关系系统（父子关系、事件系统）
- `tools_test.cpp` - 测试性能分析、内存分析、缓存分析等工具
- `debugging_test.cpp` - 测试实体浏览器、关系可视化等调试工具
- `performance_test.cpp` - 测试大规模场景下的性能表现

### 3. 快速测试
- `benchmark_test.cpp` - 提供快速的性能基准测试，适合 CI/CD 流程

### 4. 可选测试
- `profiling_test.cpp` - 深度性能分析，包含大规模测试和详细的性能指标

## 示例用法

### 运行所有 Registry 测试

```bash
./RenduCore_registry_test "[registry]"
```

### 运行性能测试

```bash
./RenduCore_performance_test "[performance]" -s
```

### 运行基准测试

```bash
./RenduCore_benchmark_test "[benchmark]"
```

### 运行所有单元测试（不包括profiling）

```bash
cmake --build cmake-build-debug --target run_unit_tests
```

## 添加新测试

1. 在 `unittests/` 目录创建新的测试文件
2. 包含 Catch2 头文件：`#include <catch2/catch_test_macros.hpp>`
3. 使用 `TEST_CASE` 宏定义测试
4. 使用 `SECTION` 组织相关测试
5. 使用 `REQUIRE` 或 `CHECK` 断言

示例：

```cpp
#include "common/ecs/registry_optimized.h"
#include <catch2/catch_test_macros.hpp>

using namespace Rendu;

struct MyComponent {
    int value;
};

TEST_CASE("My Test - Description", "[tag]") {
    RegistryOptimized registry;

    SECTION("Test case 1") {
        Entity e = registry.create();
        registry.emplaceSingle<MyComponent>(e, MyComponent{42});

        REQUIRE(registry.has<MyComponent>(e));
    }

    SECTION("Test case 2") {
        // 更多测试...
    }
}
```

## 注意事项

- 测试使用 Catch2 v3.5.0
- Catch2 通过 CMake FetchContent 自动下载
- 所有测试都是独立的，可以单独运行
- 使用 `REQUIRE` 表示关键断言（失败则停止）
- 使用 `CHECK` 表示非关键断言（继续执行）
- `profiling_test.cpp` 测试时间较长，默认不包含在 CI 中
