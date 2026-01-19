# CMake 代码规范

## 命名规范

### 1. 宏/函数命名
- **格式**: `rendu_<verb>_<object>` 或 `rendu_<verb>`
- **示例**:
  - ✅ `rendu_add_library`, `rendu_add_executable`
  - ✅ `rendu_collect_source_files`, `rendu_collect_include_directories`
  - ✅ `rendu_set_log_level`, `rendu_log_info`
  - ❌ `add_rendu_library`, `collectSources`

### 2. 变量命名
- **格式**: 全大写，下划线分隔
- **示例**:
  - ✅ `SRC_LIST`, `INCLUDE_DIRS`, `TARGET_NAME`
  - ✅ `ARG_DIR`, `ARG_NAME`, `ARG_PROJECT`
  - ❌ `srcList`, `include_dirs`, `targetName`

### 3. 常量命名
- **格式**: `<PREFIX>_<NAME>`, 全大写
- **示例**:
  - ✅ `RENDU_LOG_LEVEL_INFO`, `RENDU_CMAKE_LOG_LEVEL`
  - ✅ `RENDU_BUILDDIR`
  - ❌ `RENDU_LOG_INFO`, `LogLevel`

### 4. 选项/参数命名
- **格式**: 与函数命名一致，使用动词+名词
- **示例**:
  - ✅ `EXCLUDE_DIRS`, `EXTENSIONS`, `PRIVATE_LINK`
  - ✅ `TEST_FRAMEWORK`, `WORKING_DIR`, `TIMEOUT`
  - ❌ `excludeDirs`, `extensions`, `privateLink`

### 5. 局部变量命名
- **格式**: 小写+下划线
- **示例**:
  - ✅ `lib_type`, `visibility`, `child_path`
  - ✅ `test_command`, `alias_name`
  - ❌ `LibType`, `Visibility`, `childPath`

## 注释格式

### 1. 文件头注释
```cmake
# ====================================================================
# 模块: <ModuleName>
# 描述: <模块功能描述>
# 依赖模块:
#   - <Module1> (描述)
#   - <Module2> (描述)
#
# 使用方法: <简要说明>
# ====================================================================
```

### 2. 函数/宏注释
```cmake
# ====================================================================
# 函数: rendu_<function_name>
# 描述: <功能描述>
#
# 参数:
#   <PARAM_NAME>    - <参数描述>
#   <PARAM_NAME>    - <参数描述> (可选，默认: <默认值>)
#
# 用法示例:
#   rendu_<function_name>(
#       PARAM_NAME value1
#       PARAM_NAME value2
#   )
# ====================================================================
```

### 3. 内联注释
- 单行注释使用 `#`，与代码对齐
- 空行分隔逻辑块
- 关键步骤添加说明

```cmake
# 检查必需参数
if (NOT ARG_NAME)
    rendu_log_fatal("必须指定 NAME")
endif ()

# 收集源文件
if (NOT ARG_SOURCES)
    rendu_collect_source_files(SRC_LIST "${ARG_DIR}")
else ()
    set(SRC_LIST ${ARG_SOURCES})
endif ()
```

## 参数解析规范

### 使用 cmake_parse_arguments
```cmake
# 标准格式
set(options OPTION1 OPTION2)                          # 布尔选项
set(oneValueArgs NAME DIR PROJECT)                    # 单值参数
set(multiValueArgs SOURCES DEFINES LINKS)              # 多值参数
cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
```

### 参数命名前缀
- 统一使用 `ARG_` 前缀存储解析后的参数
- 示例: `ARG_DIR`, `ARG_NAME`, `ARG_PROJECT`, `ARG_SOURCES`

## 错误处理规范

### 1. 必需参数检查
```cmake
# 检查必需参数
if (NOT ARG_NAME)
    rendu_log_fatal("rendu_add_library: 必须指定 NAME")
endif ()
```

### 2. 默认值设置
```cmake
# 设置默认值
if (NOT ARG_TIMEOUT)
    set(ARG_TIMEOUT 300)
endif ()
```

### 3. 日志输出
```cmake
# 使用统一日志函数
rendu_log_info("添加库目标 ${target_name}")
rendu_log_debug("源文件数量: ${list_length(SRC_LIST)}")
rendu_log_warn("未指定 PROJECT，IDE 标签将缺失")
rendu_log_error("无法收集源文件")
```

## 变量作用域

### 1. 使用 PARENT_SCOPE 返回值
```cmake
function(my_function output_var)
    set(result "some_value")
    set(${output_var} ${result} PARENT_SCOPE)
endfunction()
```

### 2. 内部变量使用局部作用域
- 函数内变量默认为局部作用域
- 使用 `set(... PARENT_SCOPE)` 显式导出

## 缩进和格式

### 1. 缩进
- 使用 **4 个空格** 缩进
- 不使用 Tab

### 2. 对齐
- 函数调用参数对齐
```cmake
rendu_add_library(
    DIR       ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT   ${PROJECT_NAME}
    NAME      mylib
    ALIAS     ON
)
```

### 3. 空格
- 操作符两侧加空格: `if (condition)`, `set(var value)`
- 逗号后加空格: `list(APPEND items item1 item2)`
- 括号内侧不加空格: `function(name)`, `endif()`

## 文件组织

### 1. 模块文件结构
```cmake
# ====================================================================
# 文件头注释
# ====================================================================

# 依赖说明（如有）

# 公共常量定义（如有）

# ====================================================================
# 函数1
# ====================================================================
function(rendu_function1)
    # ...
endfunction()

# ====================================================================
# 函数2
# ====================================================================
function(rendu_function2)
    # ...
endfunction()
```

### 2. 模块顺序
- 日志系统 (RenduLogging)
- 平台检测 (RenduCheckPlatform, RenduCMakePolicies)
- 基础工具 (RenduCollectFiles, RenduCollectDirectories)
- 目标创建 (RenduAddLibrary, RenduAddExecutable, RenduAddTest)
- 项目组织 (RenduSourceGroup, RenduAddSubdirectories)
- 高级功能 (RenduConfigureTargets, RenduPCHSupport)
- 扩展功能 (RenduExtensions, RenduPackage)

## 最佳实践

### 1. 避免全局变量污染
- 使用 `CACHE INTERNAL` 存储需要全局访问的变量
- 命名使用 `RENDU_` 前缀

### 2. 提供默认值
```cmake
# 默认扩展名
if (NOT ARG_EXTENSIONS)
    set(ARG_EXTENSIONS .c .cc .cpp .h .hh .hpp)
endif ()
```

### 3. 使用列表操作
```cmake
list(APPEND items item1 item2)  # 添加元素
list(FIND items value index)    # 查找元素
list(SORT items)                 # 排序
```

### 4. 字符串处理
```cmake
string(TOLOWER "${value}" lower_value)   # 转小写
string(TOUPPER "${value}" upper_value)   # 转大写
string(REPLACE old new string result)     # 替换
```

## 禁止事项

❌ 使用 Tab 缩进
❌ 混合使用大驼峰和小驼峰命名
❌ 省略必需参数检查
❌ 隐式使用全局变量
❌ 不一致的注释格式
❌ 过度嵌套的 if 语句
