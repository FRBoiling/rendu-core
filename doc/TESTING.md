# ECS 测试说明

## 🧪 测试框架

RenduCore 使用 **Catch2** 框架编写单元测试。

### 运行所有测试

```bash
cd cmake-build-debug
make -j4
ctest --output-on-failure
```

### 运行特定测试

```bash
# 运行单个测试
./RenduCore_registry_test

# 运行特定标签
./RenduCore_registry_test "[registry][basic]"
./RenduCore_performance_test "[performance][view]"
```

### 查看测试列表

```bash
./RenduCore_registry_test --list-tests
```

---

## 📋 测试文件结构

```
src/unittests/
├── CMakeLists.txt              # 测试构建配置
├── README.md                  # 测试说明
├── registry_test.cpp           # Registry 基础测试
├── archetype_test.cpp          # Archetype 存储测试
├── signal_test.cpp            # 信号系统测试
├── migration_test.cpp         # 动态组件迁移测试
├── performance_test.cpp       # 性能测试
├── advanced_features_test.cpp # 高级功能测试
├── phase2_features_test.cpp  # Phase 2 功能测试
├── phase3_features_test.cpp  # Phase 3 功能测试
├── phase4_features_test.cpp  # Phase 4 功能测试
├── resource_test.cpp        # 资源管理测试
└── benchmark_test.cpp       # 基准测试
```

---

## 📊 测试标签

### 核心功能
- `[registry][basic]` - Registry 基础操作
- `[registry][component]` - 组件管理
- `[registry][view]` - 视图遍历
- `[archetype][basic]` - Archetype 基础
- `[archetype][batch]` - 批量操作

### 高级功能
- `[signal][construct]` - 构造信号
- `[signal][update]` - 更新信号
- `[signal][destroy]` - 销毁信号
- `[migration][add]` - 动态添加组件
- `[migration][remove]` - 动态移除组件
- `[performance][create]` - 创建性能
- `[performance][view]` - 遍历性能

### Phase 功能
- `[phase2][entity]` - Phase 2 实体功能
- `[phase2][component]` - Phase 2 组件功能
- `[phase3][profiler]` - 性能分析器
- `[phase3][thread]` - 线程安全
- `[phase3][serialization]` - 序列化
- `[phase4][browser]` - 实体浏览器
- `[phase4][analyzer]` - 内存分析器
- `[phase4][visualizer]` - 关系可视化

### 基准测试
- `[benchmark][create]` - 创建基准
- `[benchmark][iterate]` - 遍历基准

---

## 🎯 测试覆盖

### 已测试功能

#### 核心 ECS
- ✅ 实体创建、销毁、验证
- ✅ 组件添加、移除、访问
- ✅ 视图查询、遍历、过滤
- ✅ Archetype 存储和迁移
- ✅ 批量操作

#### 高级功能
- ✅ 信号系统（构造、更新、销毁）
- ✅ 动态组件管理
- ✅ 资源管理
- ✅ 关系系统
- ✅ 系统执行器

#### 调试工具
- ✅ 实体浏览器
- ✅ 性能分析器
- ✅ 内存分析器
- ✅ 缓存分析器
- ✅ 热点检测器
- ✅ 关系可视化器

#### 性能
- ✅ 实体创建性能
- ✅ 组件遍历性能
- ✅ 批量操作性能
- ✅ 缓存命中率测试

### 待完善功能

- ⚠️ 组件移除功能（部分实现）
- ⚠️ 复杂查询（any、none、排序）
- ⚠️ Archetype 迁移（部分实现）

---

## 💡 编写测试

### 基本测试结构

```cpp
#include "common/ecs/registry_optimized.h"
#include <catch2/catch_test_macros.hpp>

using namespace Rendu;

TEST_CASE("Registry - 创建实体", "[registry][basic]") {
    RegistryOptimized registry;
    
    SECTION("创建单个实体") {
        Entity e = registry.create();
        REQUIRE(registry.valid(e));
    }
    
    SECTION("创建多个实体") {
        auto entities = registry.createBatch<Position>(10);
        REQUIRE(entities.size() == 10);
        for (auto e : entities) {
            REQUIRE(registry.valid(e));
        }
    }
}
```

### 使用标签

```cpp
TEST_CASE("Performance - 视图遍历", "[performance][view]") {
    RegistryOptimized registry;
    
    // 测试代码...
}
```

### 断言宏

```cpp
REQUIRE(condition);        // 失败则停止测试
CHECK(condition);         // 失败继续执行
REQUIRE_FALSE(condition);
CHECK_FALSE(condition);
REQUIRE_EQ(a, b);
CHECK_EQ(a, b);
```

---

## 📝 测试最佳实践

1. **使用 SECTION 组织相关测试**
   ```cpp
   TEST_CASE("Test Name", "[tag]") {
       SECTION("Case 1") { /* ... */ }
       SECTION("Case 2") { /* ... */ }
   }
   ```

2. **使用适当的标签**
   ```cpp
   TEST_CASE("Test Name", "[module][feature][specific]")
   ```

3. **测试边界条件**
   ```cpp
   SECTION("空容器") { /* ... */ }
   SECTION("单个元素") { /* ... */ }
   SECTION("大量元素") { /* ... */ }
   ```

4. **测试异常情况**
   ```cpp
   SECTION("无效实体") { /* ... */ }
   SECTION("重复添加") { /* ... */ }
   ```

---

## 🚀 持续集成

测试在 CI/CD 中自动运行，确保代码质量：

```yaml
# 示例 CI 配置
steps:
  - name: Build tests
    run: cmake --build . --target run_all_tests
  
  - name: Run tests
    run: ctest --output-on-failure
```

---

**最后更新**: 2026-01-19
