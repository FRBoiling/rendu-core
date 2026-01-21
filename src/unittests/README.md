# ECS 单元测试

本目录包含使用 Catch2 v3 框架编写的 ECS 系统单元测试。

## 测试文件概览

### 核心功能测试 (5个)
- `registry_test.cpp` - Registry 核心功能测试 (基础操作、组件管理、视图遍历、分组、快照)
- `archetype_test.cpp` - Archetype 存储系统测试 (SOA 内存布局、批量操作)
- `signal_test.cpp` - 信号/观察者系统测试 (on_construct, onUpdate, onDestroy 事件)
- `resource_test.cpp` - 资源管理系统测试 (ResourceCache, Locator)
- `migration_test.cpp` - 动态组件迁移测试 (添加、移除、更新动态组件)

### 高级功能测试 (4个)
- `relations_test.cpp` - 实体关系系统测试 (父子关系、关系查询、复杂结构)
- `tools_test.cpp` - 工具系统测试 (PerformanceProfiler, MemoryAnalyzer, CacheAnalyzer, HotspotDetector, PerformanceMonitor)
- `debugging_test.cpp` - 调试和可视化工具测试 (EntityBrowser, RelationshipVisualizer)
- `performance_test.cpp` - 性能基准测试 (大规模创建、遍历、分组性能)

### 快速测试 (1个)
- `benchmark_test.cpp` - 快速基准测试 (适合 CI/CD 流程)

### 可选测试 (1个)
- `profiling_test.cpp` - 深度性能分析测试 (运行时间较长，包含详细性能指标)

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
- `registry_test.cpp` - 测试实体管理、组件操作、视图遍历、分组、快照等核心 API
- `archetype_test.cpp` - 测试 Archetype 存储和 SOA 内存布局
- `signal_test.cpp` - 测试信号系统和观察者模式（on_construct, onUpdate, onDestroy）
- `resource_test.cpp` - 测试全局资源管理（ResourceCache, Locator）
- `migration_test.cpp` - 测试动态组件的添加、移除、更新和批量操作

### 2. 高级功能测试
这些测试覆盖框架的高级特性：
- `relations_test.cpp` - 测试实体关系系统（父子关系、关系查询、复杂结构）
- `tools_test.cpp` - 测试性能分析、内存分析、缓存分析、热点检测、性能监控等工具
- `debugging_test.cpp` - 测试实体浏览器、关系可视化、导出、过滤等调试工具
- `performance_test.cpp` - 测试大规模场景下的性能表现（创建、遍历、分组、动态组件）

### 3. 快速测试
- `benchmark_test.cpp` - 提供快速的性能基准测试，适合 CI/CD 流程

### 4. 可选测试
- `profiling_test.cpp` - 深度性能分析，包含大规模测试和详细的性能指标（运行时间较长）

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

### 测试文件模板

```cpp
//
// 功能描述单元测试
//
// 测试标签说明:
// [功能][子功能1] - 测试描述1
// [功能][子功能2] - 测试描述2
//

#include "common/ecs/registry_optimized.h"
#include <catch2/catch_test_macros.hpp>

using namespace Rendu;

// 测试组件
struct MyComponent {
    int value;
};

TEST_CASE("Feature - 测试描述", "[feature][subcategory]") {
    RegistryOptimized registry;

    SECTION("测试用例1") {
        Entity e = registry.create();
        registry.emplaceSingle<MyComponent>(e, MyComponent{42});

        REQUIRE(registry.has<MyComponent>(e));
    }

    SECTION("测试用例2") {
        // 更多测试...
    }
}
```

### 测试命名规范

1. **文件命名**: 使用下划线分隔的小写字母，如 `my_feature_test.cpp`
2. **测试用例命名**: `Feature - 描述`, 如 `Registry - 基础创建和销毁`
3. **标签命名**:
   - 第一级标签: 功能模块，如 `[registry]`, `[archetype]`, `[signal]`
   - 第二级标签: 子功能，如 `[basic]`, `[component]`, `[view]`
   - 示例: `[registry][basic]`, `[signal][construct]`

### 测试编写建议

1. **使用 SECTION 组织相关测试**: 每个测试用例包含多个相关的 SECTION
2. **使用 REQUIRE 表示关键断言**: 失败则停止当前 SECTION
3. **使用 CHECK 表示非关键断言**: 失败继续执行
4. **保持测试独立性**: 每个 SECTION 应该独立运行
5. **添加清晰的注释**: 说明测试目的和预期行为

## 注意事项

- 测试框架使用 Catch2 v3.5.0
- Catch2 通过 CMake FetchContent 自动下载
- 所有测试都是独立的，可以单独运行
- 使用 `REQUIRE` 表示关键断言（失败则停止当前 SECTION）
- 使用 `CHECK` 表示非关键断言（失败继续执行）
- `profiling_test.cpp` 测试时间较长，默认不包含在 CI 中
- 所有测试文件头部都包含详细的标签说明
- 测试超时时间设置为 300 秒（profiling_test 为 600 秒）

## 测试组织结构

```
unittests/
├── CMakeLists.txt          # CMake 构建配置
├── README.md              # 本文档
├── archetype_test.cpp     # Archetype 存储测试
├── benchmark_test.cpp     # 快速基准测试
├── debugging_test.cpp     # 调试工具测试
├── migration_test.cpp     # 动态组件迁移测试
├── performance_test.cpp   # 性能基准测试
├── profiling_test.cpp     # 深度性能分析（可选）
├── registry_test.cpp      # Registry 核心测试
├── relations_test.cpp     # 关系系统测试
├── resource_test.cpp      # 资源管理测试
└── signal_test.cpp        # 信号系统测试
```

## 运行测试的最佳实践

### 日常开发
```bash
# 运行单个测试文件
./RenduCore_registry_test

# 运行特定标签的测试
./RenduCore_registry_test "[registry][basic]"
```

### 提交前检查
```bash
# 运行所有核心测试（快速）
ctest -L unit --output-on-failure
```

### CI/CD 流程
```bash
# 运行所有测试（包括性能测试，不包括 profiling）
ctest -L unit --output-on-failure
```

### 性能回归测试
```bash
# 运行快速基准测试
./RenduCore_benchmark_test "[benchmark]"

# 运行详细性能分析（可选）
./RenduCore_profiling_test "[profiling]"
```
