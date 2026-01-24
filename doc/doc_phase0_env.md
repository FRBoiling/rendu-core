# 阶段 0: 环境准备

## 目标
- 确保 CMake 构建系统正确配置
- 修复 RenduPackageManager.cmake bug
- 验证所有第三方库依赖可用

---

## 任务清单

### 1. 修复 CMake Bug
- [x] 修复 `RenduPackageManager.cmake:1247` 中 `_get_fetch_properties` 函数参数错误
- [x] 验证 CMake 配置无警告、无错误

### 2. 验证第三方库
- [x] fmt (v12.0.0) - 字符串格式化
- [x] boost (v1.90.0) - 异步 I/O、定时器
- [x] entt (v3.16.0) - ECS 框架

- [x] protobuf (v29.1) - 序列化（已修复测试禁用问题）
- [x] catch2 (v3.11.0) - 测试框架

### 3. 配置动态库
- [x] 配置 `src/common` 为动态库
- [x] 配置 `src/core` 为动态库
- [x] 配置导出宏和符号可见性

---

## 完成情况记录

### 已完成的修改

#### 1. src/common/CMakeLists.txt

- 添加了 `3rdparty::protobuf` 依赖
- 添加了 `RENDU_COMMON_EXPORT` 宏定义

#### 2. src/core/CMakeLists.txt
- 添加了 `RENDU_CORE_EXPORT` 宏定义

#### 3. src/common/include/common/define.h
添加了导出/导入宏定义：
```cpp
// Common 层 API 定义
#if defined(RENDU_COMMON_EXPORT)
  #if defined(_WIN32) || defined(_WIN64)
    #define RENDU_COMMON_API __declspec(dllexport)
  #else
    #define RENDU_COMMON_API __attribute__((visibility("default")))
  #endif
#else
  #define RENDU_COMMON_API
#endif

// Core 层 API 定义
#if defined(RENDU_CORE_EXPORT)
  #if defined(_WIN32) || defined(_WIN64)
    #define RENDU_CORE_API __declspec(dllexport)
  #else
    #define RENDU_CORE_API __attribute__((visibility("default")))
  #endif
#else
  #define RENDU_CORE_API
#endif
```

#### 4. src/core/include/core/define.h
- 添加了对 `common/define.h` 的引用

#### 5. 3rdparty/protobuf/CMakeLists.txt
- 添加了 `-Dprotobuf_BUILD_TESTS=OFF` CMake 参数，解决 googletest 缺失问题

#### 6. src/tests/ 目录结构
创建了完整的测试目录结构：
```
src/tests/
├── CMakeLists.txt
├── common/
│   ├── util/CMakeLists.txt
│   ├── io/CMakeLists.txt
│   ├── log/CMakeLists.txt
│   ├── net/CMakeLists.txt
│   ├── ser/CMakeLists.txt
│   ├── event/CMakeLists.txt
│   ├── config/CMakeLists.txt
│   └── ecs/CMakeLists.txt
└── core/
    ├── engine/CMakeLists.txt
    ├── loop/CMakeLists.txt
    ├── state/CMakeLists.txt
    ├── lifecycle/CMakeLists.txt
    └── actor/CMakeLists.txt
```

### 验证命令
```bash
cd /Users/boil/Repos/Repos-github/rendu-core
cmake -B build -S . -DRENDU_BUILD_TESTING=ON
cmake --build build
```

---

## 验收标准

### CMake 配置
```bash
cmake -B build -S .
cmake --build build
```
- [x] 无错误
- [x] 无警告（或仅忽略无法消除的第三方库警告）

### 第三方库验证
```bash
# 确保 find_package 可找到所有库
cmake --find-package -DNAME=fmt -DCOMPILER_ID=GNU -DLANGUAGE=CXX
cmake --find-package -DNAME=Boost -DCOMPILER_ID=GNU -DLANGUAGE=CXX
cmake --find-package -DNAME=entt -DCOMPILER_ID=GNU -DLANGUAGE=CXX

cmake --find-package -DNAME=Protobuf -DCOMPILER_ID=GNU -DLANGUAGE=CXX
cmake --find-package -DNAME=Catch2 -DCOMPILER_ID=GNU -DLANGUAGE=CXX
```

### 动态库导出
- [x] `librendu-common.so` / `rendu-common.dll` / `libcommon.dylib` 配置完成
- [x] `librendu-core.so` / `rendu-core.dll` / `libcore.dylib` 配置完成
- [x] 导出宏定义完成（`RENDU_COMMON_API`, `RENDU_CORE_API`）
- [x] 符号正确导出验证（Linux/macOS: `nm -D`, Windows: `dumpbin /EXPORTS`）

---

## 构建验证完成记录

**验证日期**: 2026-01-24
**验证人员**: boil
**构建环境**:
- OS: macOS (Darwin)
- 编译器: Clang 17.0.0
- CMake 版本: 4.0.3
- 构建类型: Debug
- 平台: arm64

### 构建结果

#### CMake 配置
- **状态**: ✅ 成功
- **配置时间**: 31.5 秒
- **生成时间**: 0.6 秒
- **输出目录**: `cmake-build-debug/`

#### 编译状态
- **状态**: ✅ 成功
- **编译对象**: 284 个
- **并行线程**: 8
- **编译时间**: ~2 分钟
- **无错误**: ✅

#### 测试编译
所有测试可执行文件成功生成：
- ✅ `RenduCore_string_test`
- ✅ `RenduCore_time_test`
- ✅ `RenduCore_error_test`
- ✅ `RenduCore_container_test`
- ✅ `RenduCore_io_context_test`
- ✅ `RenduCore_scheduler_test`
- ✅ `RenduCore_timer_test`
- ✅ `RenduCore_logger_test`
- ✅ `RenduCore_formatter_test`
- ✅ `RenduCore_console_sink_test`
- ✅ `RenduCore_file_sink_test`

### 单元测试结果

| 测试模块 | 测试用例 | 断言数量 | 状态 |
|---------|---------|---------|------|
| `string_test` | 9 | 52 | ✅ 通过 |
| `time_test` | 8 | 17 | ✅ 通过 |
| `error_test` | 8 | 31 | ✅ 通过 |
| `container_test` | 16 | 42 | ✅ 通过 |
| `io_context_test` | 4 | 7 | ✅ 通过 |
| `scheduler_test` | 4 | 5 | ✅ 通过 |
| `timer_test` | 5 | 11 | ✅ 通过 |
| `logger_test` | 11 | 11 | ✅ 通过 |
| `formatter_test` | 8 | 21 | ✅ 通过 |
| `console_sink_test` | 3 | 4 | ✅ 通过 |
| `file_sink_test` | 2 | 5 | ✅ 通过 |

**总计**: 10 个测试 | 76 个测试用例 | 206 个断言 | **100% 通过率** ✅

### 验证命令
```bash
# 配置项目
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug

# 编译项目
cmake --build cmake-build-debug -- -j8

# 运行测试
for test in cmake-build-debug/src/tests/common/*/*_test; do
    echo "=== Running $test ==="
    $test 2>&1 | grep -E "(All tests passed|FAILED)"
done
```

### 修复的问题

#### 1. Logger 命名空间问题
- **文件**: `src/common/include/common/log/logger.h`
- **问题**: 模板实现使用了错误的命名空间声明 `namespace rendu::log`
- **修复**: 改为使用 `BEGIN_NAMESPACE_COMMON` 宏
- **影响**: 修复编译错误

#### 2. IoContext 类型引用问题
- **文件**: `src/common/include/common/log/logger.h`, `src/common/src/log/logger.cpp`
- **问题**: `log` 命名空间中引用 `IoContext` 时缺少完整路径
- **修复**: 将所有 `IoContext` 改为 `io::IoContext`
- **影响**: 修复链接错误

#### 3. log_fields 模板实现
- **文件**: `src/common/include/common/log/logger.h`
- **问题**: fold expression 实现不正确导致编译错误
- **修复**: 使用 if constexpr 手动展开参数对（支持最多 10 个参数）
- **影响**: 修复模板编译错误

#### 4. Example 项目问题
- **文件**: `src/apps/example/main.cpp`, `src/apps/example/example.h`
- **问题**: 引用不存在的 `using namespace Rendu;` 和缺失的 `example.h`
- **修复**: 移除错误命名空间，创建占位符头文件
- **影响**: 修复 example 编译错误

### 验证结论

✅ **阶段 0 构建验证完全通过**

- 所有依赖正确配置
- 代码编译无错误
- 所有测试 100% 通过
- 项目构建系统运行正常
- 动态库导出配置正确

---

## 单元测试

### 测试文件结构
```
src/tests/
├── CMakeLists.txt
├── common/
│   ├── util/
│   ├── io/
│   ├── log/
│   ├── net/
│   ├── ser/
│   ├── event/
│   ├── config/
│   └── ecs/
└── core/
    ├── engine/
    ├── loop/
    ├── state/
    ├── lifecycle/
    └── actor/
```

### CMakeLists.txt
总配置文件 `src/tests/CMakeLists.txt`:
```cmake
rendu_add_subdirectories(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    EXCLUDE_DIRS
)
```

每个测试子目录都有占位 CMakeLists.txt，等待后续阶段添加测试用例。

### 测试用例
本阶段为环境准备，暂无单元测试。仅验证构建系统正确性。

---

## 技术要点

### 1. CMake 版本要求
```cmake
cmake_minimum_required(VERSION 3.20)
project(RenduCore VERSION 1.0.0 LANGUAGES CXX)
```

### 2. C++ 标准
```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

### 3. 动态库导出宏
```cpp
// src/common/include/common/define.h
#if defined(_WIN32) || defined(_WIN64)
  #ifdef RENDU_COMMON_EXPORT
    #define RENDU_COMMON_API __declspec(dllexport)
  #else
    #define RENDU_COMMON_API __declspec(dllimport)
  #endif
#else
  #define RENDU_COMMON_API __attribute__((visibility("default")))
#endif

// src/core/include/core/define.h
#if defined(_WIN32) || defined(_WIN64)
  #ifdef RENDU_CORE_EXPORT
    #define RENDU_CORE_API __declspec(dllexport)
  #else
    #define RENDU_CORE_API __declspec(dllimport)
  #endif
#else
  #define RENDU_CORE_API __attribute__((visibility("default")))
#endif
```

---

## 下一步
完成本阶段后，进入 **阶段 1: Common 层 - 基础工具 (util)**
