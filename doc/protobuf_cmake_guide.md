# 使用 rendu_add_library 编译 Protobuf C++ 库指南

## 目录
- [概述](#概述)
- [rendu_add_library 参数详解](#rendu_add_library-参数详解)
- [编译 Protobuf 库](#编译-protobuf-库)
- [跨平台配置](#跨平台配置)
- [常见问题解决](#常见问题解决)

---

## 概述

`rendu_add_library` 是 RenduCore 项目提供的增强版 `add_library` 宏，它：

1. **自动收集源文件** - 无需手动列出所有 .cpp/.h 文件
2. **自动管理 include 目录** - 自动收集头文件路径
3. **标准化目标属性** - 统一的项目结构和标签
4. **支持 ALIAS 目标** - 提供更清晰的依赖命名

---

## rendu_add_library 参数详解

### 必需参数

| 参数 | 类型 | 说明 | 示例 |
|------|------|------|--------|
| `DIR` | 路径 | 源码根目录（绝对路径或 `${CMAKE_CURRENT_SOURCE_DIR}`） | `${CMAKE_CURRENT_SOURCE_DIR}` |
| `NAME` | 字符串 | 目标库名称 | `protobuf` |

### 可选参数

| 参数 | 类型 | 说明 | 默认值 |
|------|------|------|--------|
| `PROJECT` | 字符串 | 项目名（用于 IDE 分组和 ALIAS） | `rendu-core` |
| `PRIVATE_LINK` | 列表 | PRIVATE 链接的库 | - |
| `PUBLIC_LINK` | 列表 | PUBLIC 链接的库（传递给依赖者） | - |
| `DEFINES` | 列表 | 预处理宏定义 | - |
| `SOURCES` | 列表 | 手动指定源文件（不自动收集） | - |
| `STATIC` | 标志 | 创建静态库 | 不指定时默认为 STATIC |
| `SHARED` | 标志 | 创建动态库（.so/.dll/.dylib） | - |
| `INTERFACE` | 标志 | 创建接口库（header-only） | - |
| `ALIAS` | 标志 | 创建 `PROJECT::NAME` 形式的 ALIAS | `ON` |

---

## 编译 Protobuf 库

### 基本用法

```cmake
# 3rdparty/protobuf/CMakeLists.txt

cmake_minimum_required(VERSION 3.24)
project(rendu-protobuf LANGUAGES CXX)

# 加载 RenduCore 宏
include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/RenduCore.cmake)

# 编译 Protobuf C++ 库
rendu_add_library(
    DIR          ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT      ${PROJECT_NAME}
    NAME         protobuf
    PUBLIC_LINK  pthread          # Linux/macOS 需要
    ALIAS        ON
)
```

### 完整配置示例

```cmake
# 3rdparty/protobuf/CMakeLists.txt

cmake_minimum_required(VERSION 3.24)
project(rendu-protobuf LANGUAGES CXX)

# 加载 RenduCore 宏
include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/RenduCore.cmake)

# 编译选项
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Protobuf 特定宏
if(MSVC)
    add_compile_options(/wd4065 /wd4146 /wd4244 /wd4251)
elseif(NOT MSVC)
    add_compile_options(-Wno-sign-compare)
endif()

# 编译 Protobuf 库
rendu_add_library(
    DIR          ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT      ${PROJECT_NAME}
    NAME         protobuf
    PRIVATE_LINK rendu-core-interface  # 接口库
    PUBLIC_LINK  pthread           # 跨平台线程库
    DEFINES      GOOGLE_PROTOBUF_NO_RTTI  # 禁用 RTTI（性能优化）
    ALIAS        ON
)

# 设置输出名称
set_target_properties(rendu-protobuf PROPERTIES
    OUTPUT_NAME "protobuf"
    VERSION "33.4"
    SOVERSION "33"
)
```

---

## 跨平台配置

### 平台检测宏

`rendu_add_library` 依赖 CMake 内置平台变量，使用以下宏进行跨平台判断：

| 宏 | 说明 |
|-----|------|
| `WIN32` 或 `MSVC` | Windows 系统 |
| `APPLE` | macOS/iOS 系统 |
| `UNIX` | Unix-like 系统（包括 Linux/macOS） |
| `LINUX` | Linux 系统 |
| `ANDROID` | Android 系统 |

### Windows 配置

```cmake
if(WIN32 OR MSVC)
    # Windows 特定配置
    target_compile_definitions(rendu-protobuf PRIVATE
        _WIN32
        _WINDOWS
        NOMINMAX          # 避免与 std::min/max 冲突
        WIN32_LEAN_AND_MEAN
    )

    # Windows 链接库
    target_link_libraries(rendu-protobuf PRIVATE
        ws2_32          # Windows Socket 库
        bcrypt           # 密码学支持
    )

    # MSVC 特定选项
    if(MSVC)
        target_compile_options(rendu-protobuf PRIVATE
            /W4              # 警告级别 4
            /utf-8            # UTF-8 源码
            /MP               # 多进程编译
            /wd4100          # 忽略类型转换警告
            /wd4127          # 忽略字符截断警告
        )
    endif()
endif()
```

### Linux 配置

```cmake
if(LINUX)
    # Linux 特定配置
    target_compile_definitions(rendu-protobuf PRIVATE
        _LINUX
        _GNU_SOURCE
    )

    # Linux 链接库
    target_link_libraries(rendu-protobuf PRIVATE
        pthread           # 线程库
        rt                # 实时扩展
        dl                # 动态加载
    )

    # 编译选项
    target_compile_options(rendu-protobuf PRIVATE
        -fPIC              # 位置无关代码（共享库必需）
        -Wall               # 所有警告
        -Wextra             # 额外警告
    )
endif()
```

### macOS 配置

```cmake
if(APPLE)
    # macOS 特定配置
    target_compile_definitions(rendu-protobuf PRIVATE
        _APPLE
        __MACOSX__
    )

    # macOS 链接库和框架
    target_link_libraries(rendu-protobuf PRIVATE
        pthread
        "-framework CoreFoundation"
    )

    # macOS 特定选项
    target_compile_options(rendu-protobuf PRIVATE
        -fPIC
    )

    # 部署目标
    if(IOS)
        set_target_properties(rendu-protobuf PROPERTIES
            MACOSX_BUNDLE TRUE
            XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        )
    endif()
endif()
```

### 通用跨平台配置

```cmake
# 线程库（所有平台）
if(WIN32 OR MSVC)
    set(THREAD_LIB "")
elseif(APPLE)
    set(THREAD_LIB "-lpthread -lm -framework CoreFoundation")
else()
    set(THREAD_LIB "-lpthread -lm")
endif()

# 链接线程库
target_link_libraries(rendu-protobuf PUBLIC ${THREAD_LIB})
```

---

## 常见问题解决

### 问题 1: 找不到 pthread 头文件

**错误信息：**
```
fatal error: pthread.h: No such file or directory
```

**解决方案：**
```cmake
if(UNIX AND NOT APPLE)
    target_link_libraries(rendu-protobuf PRIVATE pthread)
endif()
```

### 问题 2: Windows 上链接错误

**错误信息：**
```
unresolved external symbol __imp_*
```

**解决方案：**
```cmake
if(WIN32)
    target_link_libraries(rendu-protobuf PRIVATE
        ws2_32
        bcrypt
    )
endif()
```

### 问题 3: macOS 上符号冲突

**错误信息：**
```
duplicate symbol for operator<<(std::ostream&)
```

**解决方案：**
```cmake
if(APPLE)
    target_compile_definitions(rendu-protobuf PRIVATE
        NOMINMAX
        _DARWIN_C_SOURCE
    )
endif()
```

### 问题 4: 共享库需要 fPIC

**错误信息：**
```
relocation R_X86_64_32 against symbol ... can not be used when making a shared object
```

**解决方案：**
```cmake
if(BUILD_SHARED_LIBS)
    target_compile_options(rendu-protobuf PRIVATE -fPIC)
endif()
```

### 问题 5: 编译器版本不兼容

**错误信息：**
```
error: 'constexpr' does not name a type
```

**解决方案：**
```cmake
# 确保使用 C++17 或更高
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 或指定特定编译器要求
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(rendu-protobuf PRIVATE
        -std=c++17
    )
endif()
```

### 问题 6: RTTI 相关问题

**错误信息：**
```
error: cannot use typeid with -fno-rtti
```

**解决方案：**
```cmake
# 禁用 RTTI（如果 Protobuf 使用了）
target_compile_definitions(rendu-protobuf PRIVATE
    GOOGLE_PROTOBUF_NO_RTTI
)
```

### 问题 7: 头文件找不到

**错误信息：**
```
fatal error: google/protobuf/message.h: No such file or directory
```

**解决方案：**
```cmake
# 手动添加 include 目录
target_include_directories(rendu-protobuf PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
```

### 问题 8: 多定义错误

**错误信息：**
```
multiple definition of symbol ...
```

**解决方案：**
```cmake
# 确保只在一个地方定义符号
set_target_properties(rendu-protobuf PROPERTIES
    POSITION_INDEPENDENT_CODE ON  # -fPIC
    C_VISIBILITY_PRESET hidden      # -fvisibility=hidden
)

# 导出符号（动态库）
if(BUILD_SHARED_LIBS)
    target_compile_definitions(rendu-protobuf PUBLIC
        PROTOBUF_EXPORT
    )
endif()
```

---

## 完整示例

### 示例 1: 静态库（默认）

```cmake
rendu_add_library(
    DIR          ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT      rendu-core
    NAME         protobuf
    PUBLIC_LINK  pthread
    ALIAS        ON
)
```

生成目标：
- `rendu-protobuf` (静态库，.a/.lib)
- `rendu-core::protobuf` (ALIAS)

### 示例 2: 动态库

```cmake
rendu_add_library(
    DIR          ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT      rendu-core
    NAME         protobuf
    SHARED              # 创建动态库
    PUBLIC_LINK  pthread
    DEFINES      PROTOBUF_EXPORT  # 导出符号
    ALIAS        ON
)
```

生成目标：
- `rendu-protobuf` (动态库，.so/.dll/.dylib)
- `rendu-core::protobuf` (ALIAS)

### 示例 3: 接口库

```cmake
rendu_add_library(
    DIR          ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT      rendu-core
    NAME         protobuf-lite
    INTERFACE           # header-only 库
    PRIVATE_LINK rendu-core-interface
    ALIAS        ON
)
```

### 示例 4: 跨平台配置

```cmake
# 3rdparty/protobuf/CMakeLists.txt

cmake_minimum_required(VERSION 3.24)
project(rendu-protobuf LANGUAGES CXX)

include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/RenduCore.cmake)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 跨平台配置
if(WIN32 OR MSVC)
    target_compile_definitions(rendu-protobuf PRIVATE
        _WIN32 NOMINMAX WIN32_LEAN_AND_MEAN
    )
    target_link_libraries(rendu-protobuf PRIVATE ws2_32 bcrypt)
elseif(APPLE)
    target_link_libraries(rendu-protobuf PRIVATE
        "-lpthread -lm -framework CoreFoundation"
    )
else()
    target_link_libraries(rendu-protobuf PRIVATE
        "-lpthread -lm -lrt -ldl"
    )
endif()

# 编译 Protobuf
rendu_add_library(
    DIR          ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT      ${PROJECT_NAME}
    NAME         protobuf
    PUBLIC_LINK  pthread
    DEFINES      GOOGLE_PROTOBUF_NO_RTTI
    ALIAS        ON
)
```

---

## 总结

`rendu_add_library` 简化了 C++ 库的编译流程：

1. ✅ **自动源文件收集** - 无需手动维护文件列表
2. ✅ **自动 include 管理** - 头文件路径自动收集
3. ✅ **跨平台支持** - 通过 CMake 平台宏实现
4. ✅ **依赖管理** - PRIVATE/PUBLIC 链接区分清晰
5. ✅ **ALIAS 目标** - 提供清晰的依赖命名 `project::name`

**关键要点：**
- 使用 `CMAKE_CURRENT_SOURCE_DIR` 作为源码目录
- 使用 `PUBLIC_LINK` 传递依赖给使用者
- 使用 `PRIVATE_LINK` 仅在内部使用依赖
- 使用平台宏（`WIN32`、`APPLE`、`LINUX`）实现跨平台
- 使用 `ALIAS ON` 创建标准的命名目标
