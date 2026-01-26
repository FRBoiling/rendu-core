# Protobuf C++ 库源代码获取指南

## 问题说明

在之前的清理过程中，Protobuf 的 C++ 源代码文件（.cc 和 .h）被意外删除。当前 `3rdparty/protobuf/` 目录仅保留了 CMake 配置和部分第三方依赖，但缺少核心的 C++ 实现文件。

## 解决方案

### 方案一：从官方 Protobuf 仓库下载（推荐）

```bash
# 进入项目目录
cd /Users/boil/Repos/Repos-github/rendu-core/3rdparty

# 备份当前 protobuf 文件夹（如果需要）
mv protobuf protobuf_backup

# 克隆 Protobuf 官方仓库（v21.x 或 v22.x 版本）
git clone --depth=1 --branch v21.12 https://github.com/protocolbuffers/protobuf.git

# 或者使用 wget 下载特定版本的压缩包
wget https://github.com/protocolbuffers/protobuf/releases/download/v21.12/protobuf-cpp-21.12.tar.gz
tar -xzf protobuf-cpp-21.12.tar.gz
mv protobuf-21.12 protobuf
```

### 方案二：使用 CMake 的 FetchContent（自动化）

在项目根目录的 `CMakeLists.txt` 中添加：

```cmake
include(FetchContent)

# 设置 Protobuf 版本
set(PROTOBUF_VERSION "21.12")

# 配置 Protobuf 选项以避免编译测试和示例
set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_CONFORMANCE OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_PROTOC_BINARIES ON CACHE BOOL "" FORCE)
set(protobuf_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS} CACHE BOOL "" FORCE)

# 下载 Protobuf
FetchContent_Declare(
    protobuf
    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
    GIT_TAG v${PROTOBUF_VERSION}
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(protobuf)

# 创建别名目标以便于使用
add_library(Protobuf::libprotobuf ALIAS protobuf::libprotobuf)
```

### 方案三：从系统已安装的 Protobuf 使用

如果系统已安装 Protobuf：

```bash
# macOS (使用 Homebrew)
brew install protobuf

# Linux (Ubuntu/Debian)
sudo apt-get install libprotobuf-dev protobuf-compiler

# Linux (CentOS/RHEL)
sudo yum install protobuf-devel protobuf-compiler
```

然后在 CMake 中：

```cmake
find_package(Protobuf REQUIRED)
add_executable(your_target main.cpp)
target_link_libraries(your_target protobuf::libprotobuf)
```

## 下载后清理

下载完整的 Protobuf 源代码后，需要清理不必要的文件以减小体积：

```bash
cd 3rdparty/protobuf

# 删除测试相关
find . -type f \( -name "*test*" -o -name "*unittest*" -o -name "*benchmark*" \) -delete
rm -rf src/google/protobuf/testdata

# 删除非 C++ 语言支持
find . -type d \( -name "*ruby*" -o -name "*python*" -o -name "*java*" -o -name "*go*" -o -name "*php*" -o -name "*cs*" -o -name "*rust*" \) -exec rm -rf {} + 2>/dev/null

# 删除其他不需要的文件
rm -rf examples
rm -rf conformance
rm -f .gitignore
rm -rf .github
```

## 验证源代码完整性

下载后验证关键的源文件是否存在：

```bash
# 检查是否有 .cc 源文件
ls src/google/protobuf/*.cc | wc -l
# 应该有 80+ 个 .cc 文件

# 检查是否有 .h 头文件
ls src/google/protobuf/*.h | wc -l
# 应该有 80+ 个 .h 文件
```

## 使用 rendu_add_library 编译

获取源代码后，使用现有的 `3rdparty/protobuf/CMakeLists.txt` 配置即可编译：

```bash
# 从项目根目录构建
cd /Users/boil/Repos/Repos-github/rendu-core

# 创建构建目录
mkdir -p build && cd build

# 配置 CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译 Protobuf 库
cmake --build . --target rendu-protobuf
```

## 关键文件列表

确保以下文件存在于 `src/google/protobuf/` 目录：

核心源文件 (.cc)：
- any.cc, arena.cc, arenastring.cc
- descriptor.cc, descriptor_database.cc, dynamic_message.cc
- extension_set.cc, extension_set_heavy.cc
- generated_message_*.cc (多个文件)
- io/coded_stream.cc, io/zero_copy_stream*.cc
- json/*.cc (多个文件)
- map.cc, map_field.cc
- message.cc, message_lite.cc
- repeated_field.cc, repeated_ptr_field.cc
- text_format.cc, wire_format*.cc

预生成文件 (.pb.cc)：
- any.pb.cc, api.pb.cc, duration.pb.cc
- descriptor.pb.cc
- timestamp.pb.cc, type.pb.cc, wrappers.pb.cc

头文件 (.h)：
- 对应的 .h 文件

## 推荐版本

- **稳定版**: v21.12 (推荐，功能完整)
- **最新版**: v23.x 或 v24.x (如需最新特性)

## 注意事项

1. Protobuf 需要 C++17 或更高版本
2. Windows 下需要设置正确的运行时库
3. 如果启用共享库，需要导出符号
4. 推荐禁用 RTTI 以提升性能

## 故障排查

如果编译失败：

1. **找不到头文件**: 确保 `src/google/protobuf/` 路径正确
2. **链接错误**: 检查平台特定的链接库（pthread, rt, dl 等）
3. **C++ 标准问题**: 确保编译器支持 C++17
4. **文件缺失**: 验证是否成功下载了所有源文件
