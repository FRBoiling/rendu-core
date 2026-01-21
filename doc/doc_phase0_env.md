# 阶段 0: 环境准备

## 目标
- 确保 CMake 构建系统正确配置
- 修复 RenduPackageManager.cmake bug
- 验证所有第三方库依赖可用

---

## 任务清单

### 1. 修复 CMake Bug
- [ ] 修复 `RenduPackageManager.cmake:1247` 中 `_get_fetch_properties` 函数参数错误
- [ ] 验证 CMake 配置无警告、无错误

### 2. 验证第三方库
- [ ] fmt - 字符串格式化
- [ ] boost - 异步 I/O、定时器
- [ ] entt - ECS 框架
- [ ] spdlog - 日志库
- [ ] protobuf - 序列化
- [ ] catch2 - 测试框架

### 3. 配置动态库
- [ ] 配置 `src/common` 为动态库
- [ ] 配置 `src/core` 为动态库
- [ ] 配置导出宏和符号可见性

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
- `librendu-common.so` / `rendu-common.dll` 可生成
- `librendu-core.so` / `rendu-core.dll` 可生成
- 符号正确导出（Linux/macOS: `nm -D`, Windows: `dumpbin /EXPORTS`）

---

## 单元测试

### 测试文件结构
```
src/tests/
└── CMakeLists.txt
```

### CMakeLists.txt
```cmake
rendu_add_subdirectories(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
)
```

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
