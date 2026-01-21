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
- [x] spdlog (v1.15.0) - 日志库
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
- 添加了 `3rdparty::spdlog` 依赖
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
- 无错误
- 无警告（或仅忽略无法消除的第三方库警告）

### 第三方库验证
```bash
# 确保 find_package 可找到所有库
cmake --find-package -DNAME=fmt -DCOMPILER_ID=GNU -DLANGUAGE=CXX
cmake --find-package -DNAME=Boost -DCOMPILER_ID=GNU -DLANGUAGE=CXX
cmake --find-package -DNAME=entt -DCOMPILER_ID=GNU -DLANGUAGE=CXX
cmake --find-package -DNAME=spdlog -DCOMPILER_ID=GNU -DLANGUAGE=CXX
cmake --find-package -DNAME=Protobuf -DCOMPILER_ID=GNU -DLANGUAGE=CXX
cmake --find-package -DNAME=Catch2 -DCOMPILER_ID=GNU -DLANGUAGE=CXX
```

### 动态库导出
- [x] `librendu-common.so` / `rendu-common.dll` / `libcommon.dylib` 配置完成
- [x] `librendu-core.so` / `rendu-core.dll` / `libcore.dylib` 配置完成
- [x] 导出宏定义完成（`RENDU_COMMON_API`, `RENDU_CORE_API`）
- [ ] 符号正确导出验证（Linux/macOS: `nm -D`, Windows: `dumpbin /EXPORTS`）- 待构建完成验证

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
