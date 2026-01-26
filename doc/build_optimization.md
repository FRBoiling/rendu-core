# 构建优化指南

## 问题

每次 CMake 配置时，系统会尝试从 GitHub 下载 protobuf 和 abseil，即使这些包已经在缓存中。

## 解决方案

### 方案1：使用本地包（推荐）

在 CMake 配置时使用 `-DRENDU_USE_LOCAL_PACKAGES=ON` 选项：

```bash
cmake -B cmake-build-debug -S . -DCMAKE_BUILD_TYPE=Debug -DRENDU_USE_LOCAL_PACKAGES=ON
```

这个选项会让包管理器优先使用 `find_package` 查找已安装或缓存的包。

### 方案2：只使用本地包

如果不想有任何网络下载，使用 `-DRENDU_LOCAL_PACKAGES_ONLY=ON`：

```bash
cmake -B cmake-build-debug -S . -DCMAKE_BUILD_TYPE=Debug -DRENDU_LOCAL_PACKAGES_ONLY=ON
```

注意：这要求所有依赖都已安装或缓存。

### 方案3：设置缓存目录

包缓存默认位于 `~/.rdpm/packages`。可以设置环境变量 `RENDU_PACKAGES_CACHE`：

```bash
export RENDU_PACKAGES_CACHE=/path/to/cache
cmake -B cmake-build-debug -S . -DCMAKE_BUILD_TYPE=Debug
```

### 方案4：使用包锁

使用 `rendu-package-lock.cmake` 锁定包版本，避免重复下载：

```bash
# 首次配置后会生成锁文件
cmake -B cmake-build-debug -S . -DCMAKE_BUILD_TYPE=Debug

# 后续配置使用锁文件
cmake -B cmake-build-debug -S . -DCMAKE_BUILD_TYPE=Debug -DRENDU_PACKAGE_LOCK_ENABLED=ON
```

### 方案5：针对特定包禁用下载

设置环境变量禁用特定包的下载：

```bash
# 禁用 protobuf 下载
export RENDU_DOWNLOAD_Protobuf=OFF

# 禁用 abseil 下载
export RENDU_DOWNLOAD_absl=OFF

cmake -B cmake-build-debug -S . -DCMAKE_BUILD_TYPE=Debug
```

## 推荐配置

对于开发环境，推荐使用方案1（本地包优先）：

```bash
rm -rf cmake-build-debug
cmake -B cmake-build-debug -S . \
  -DCMAKE_BUILD_TYPE=Debug \
  -DRENDU_USE_LOCAL_PACKAGES=ON
```

这样可以：
1. 首次下载后会缓存到 `~/.rdpm/packages`
2. 后续配置优先使用缓存
3. 需要时仍可下载新版本

## 查看包缓存

```bash
# 查看缓存目录内容
ls -la ~/.rdpm/packages/

# 查看特定包
ls -la ~/.rdpm/packages/protobuf/
ls -la ~/.rdpm/packages/absl/
```

## 清理缓存

如需清理缓存重新下载：

```bash
# 清理特定包
rm -rf ~/.rdpm/packages/protobuf/
rm -rf ~/.rdpm/packages/absl/

# 清理所有缓存
rm -rf ~/.rdpm/packages/
```

## 当前项目使用的包

- **protobuf**: v33.4.0 (protocolbuffers/protobuf)
- **abseil**: 20250512.1 (abseil/abseil-cpp)
- **boost**: v1.90.0
- **entt**: v3.16.0
- **fmt**: v12.0.0
- **catch2**: v3.11.0
