# CMake 目录结构说明

## 目录结构

```
cmake/
├── RenduCore.cmake              # 统一入口模块 (推荐使用)
├── CODING_STANDARDS.md          # 代码规范文档
├── genrev.cmake                 # 版本号生成
├── options.cmake                # 构建选项定义
├── showoptions.cmake            # 显示构建选项
│
├── macros/                      # CMake 宏模块
│   ├── 基础模块
│   │   ├── RenduLogging.cmake            # 日志系统 (必须最先加载)
│   │   ├── RenduCMakePolicies.cmake     # CMake 策略设置
│   │   ├── RenduCheckPlatform.cmake      # 平台检测
│   │   └── RenduCheckBuildDir.cmake      # 构建目录检查
│   │
│   ├── 源文件收集
│   │   ├── RenduCollectFiles.cmake       # 源文件收集
│   │   └── RenduCollectDirectories.cmake # 目录收集
│   │
│   ├── 目标构建
│   │   ├── RenduAddExecutable.cmake      # 可执行目标
│   │   ├── RenduAddLibrary.cmake         # 库目标
│   │   ├── RenduAddTest.cmake            # 测试目标
│   │   └── RenduAddCustomTarget.cmake    # 自定义目标
│   │
│   ├── 项目组织
│   │   ├── RenduAddSubdirectories.cmake   # 子目录添加
│   │   ├── RenduSourceGroup.cmake         # 源文件分组
│   │   └── RenduProject.cmake            # 项目设置
│   │
│   ├── 高级功能
│   │   ├── RenduConfigureTargets.cmake    # 目标配置
│   │   ├── RenduPCHSupport.cmake         # 预编译头支持
│   │   ├── ConfigureScripts.cmake         # 脚本配置
│   │   └── EnsureVersion.cmake           # 版本检查
│   │
│   └── 扩展功能
│       ├── RenduExtensions.cmake         # 安装功能
│       ├── RenduPackage.cmake            # 打包配置
│       └── RenduPackageManager.cmake     # 包管理 (44KB)
│
├── compiler/                    # 编译器相关配置
│   ├── clang/
│   ├── gcc/
│   └── msvc/
│
└── platform/                    # 平台相关配置
    ├── cmake_uninstall.in.cmake # 卸载脚本模板
    ├── unix/
    └── win/
```

## 使用方法

### 方式一：使用统一入口 (推荐)

```cmake
# 在顶层 CMakeLists.txt 中
include(${CMAKE_SOURCE_DIR}/cmake/RenduCore.cmake)

# 然后直接使用各种宏
rendu_add_library(
    DIR ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT ${PROJECT_NAME}
    NAME mylib
    ALIAS ON
)
```

### 方式二：按需加载

```cmake
# 只加载需要的模块 (需确保依赖已加载)
include(RenduLogging)
include(RenduCollectFiles)
include(RenduCollectDirectories)
include(RenduAddLibrary)
include(RenduAddExecutable)
```

## 代码规范

详细的编码规范请参阅 [CODING_STANDARDS.md](CODING_STANDARDS.md)，主要内容包括：

### 命名规范

| 类型 | 格式 | 示例 |
|------|------|------|
| 宏/函数 | `rendu_<verb>_<object>` | `rendu_add_library`, `rendu_collect_source_files` |
| 变量 | 全大写，下划线分隔 | `SRC_LIST`, `INCLUDE_DIRS`, `TARGET_NAME` |
| 常量 | `<PREFIX>_<NAME>` | `RENDU_LOG_LEVEL_INFO`, `RENDU_CMAKE_LOG_LEVEL` |
| 参数 | 与函数命名一致 | `EXCLUDE_DIRS`, `EXTENSIONS`, `PRIVATE_LINK` |
| 局部变量 | 小写+下划线 | `lib_type`, `visibility`, `child_path` |

### 注释格式

```cmake
# ====================================================================
# 模块: RenduModuleName
# 描述: 模块功能描述
# 依赖模块:
#   - RenduLogging (日志)
#   - RenduCollectFiles (文件收集)
# ====================================================================

# ====================================================================
# 函数: rendu_function_name
# 描述: 函数功能描述
#
# 参数:
#   PARAM_NAME    - 参数描述 (可选，默认: 默认值)
#   PARAM_NAME    - 参数描述
# ====================================================================
```

## 模块依赖关系

```
RenduLogging (必须最先)
    ↓
RenduCMakePolicies, RenduCheckPlatform, RenduCheckBuildDir
    ↓
RenduCollectFiles, RenduCollectDirectories
    ↓
RenduAddExecutable, RenduAddLibrary, RenduAddTest, RenduAddCustomTarget
    ↓
RenduAddSubdirectories, RenduSourceGroup, RenduProject
    ↓
RenduConfigureTargets, RenduPCHSupport, ConfigureScripts, EnsureVersion
    ↓
RenduExtensions
    ↓
RenduPackage, RenduPackageManager (可选)
```

## 空目录说明

以下目录目前为空，可根据需要添加平台/编译器特定配置：
- `compiler/clang/`
- `compiler/gcc/`
- `compiler/msvc/`
- `platform/unix/`
- `platform/win/`

## 常用宏速查

| 宏 | 功能 | 示例 |
|---|---|---|
| `rendu_add_library` | 创建库目标，自动收集源文件 | 见文档 |
| `rendu_add_executable` | 创建可执行目标，自动收集源文件 | 见文档 |
| `rendu_add_test` | 创建测试目标，支持 Catch2/GTest | 见文档 |
| `rendu_add_custom_target` | 创建自定义构建目标 | 见文档 |
| `rendu_add_custom_command` | 创建自定义命令 | 见文档 |
| `rendu_collect_source_files` | 递归收集源文件 | 见文档 |
| `rendu_collect_header_files` | 递归收集头文件 | 见文档 |
| `rendu_collect_include_directories` | 收集包含目录 | 见文档 |
| `rendu_source_group` | 为源文件创建 IDE 分组 | 见文档 |
| `rendu_install_targets` | 批量安装目标 | 见文档 |
| `rendu_add_uninstall_target` | 添加卸载目标 | 见文档 |

## 优化内容

### 已完成

1. **统一入口** - 创建 `RenduCore.cmake` 统一加载所有模块
2. **移除冗余 include** - 各模块移除重复的 include 语句
3. **统一命名规范** - 所有变量、函数、参数符合编码规范
4. **统一注释格式** - 所有函数使用统一的注释模板
5. **代码格式化** - 统一缩进、空格、对齐
6. **错误处理** - 统一使用日志函数输出错误信息
7. **参数解析** - 统一使用 `ARG_` 前缀存储解析后的参数
8. **默认值设置** - 为可选参数提供合理的默认值

### 代码质量

- ✅ 无新增 linter 错误
- ✅ 所有模块符合编码规范
- ✅ 注释完整且格式统一
- ✅ 命名清晰且一致
- ✅ 依赖关系明确

