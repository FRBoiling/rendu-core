# ====================================================================
# 模块: options
# 描述: RenduCore 构建选项配置,定义所有 CMake 构建选项
# 依赖模块:
#   - ConfigureScripts (脚本配置)
# ====================================================================

# ====================================================================
# 基础构建选项
# ====================================================================
# 是否构建 launcher 可执行文件
option(RENDU_LAUCHER "构建 launcher" ON)
# 工具链相关选项
option(RENDU_TOOLS "构建工具" ON)
# 测试相关选项
option(BUILD_TESTING "构建测试套件" ON)
set(RENDU_BUILD_TESTING ${BUILD_TESTING})
# 示例相关选项
option(RENDU_BUILD_EXAMPLES "构建示例程序" ON)

# ====================================================================
# 脚本构建模式可选项
# 说明: none=不构建脚本, static=静态链接脚本, dynamic=动态链接脚本, custom=自定义模式
# ====================================================================
# 定义 RENDU_SCRIPTS 可选值
set(RENDU_SCRIPTS_AVAILABLE_OPTIONS "none" "static" "dynamic" "custom" CACHE STRING "RENDU_SCRIPTS 可选值列表" FORCE)

# 定义 RENDU_SCRIPTS 选项
if (NOT DEFINED RENDU_SCRIPTS)
    set(RENDU_SCRIPTS "none" CACHE STRING "脚本构建模式 (none|static|dynamic|custom)")
endif ()
set_property(CACHE RENDU_SCRIPTS PROPERTY STRINGS "none" "static" "dynamic" "custom")

# ====================================================================
# 校验 RENDU_SCRIPTS 变量是否合法
# ====================================================================
if (RENDU_SCRIPTS)
    list(FIND RENDU_SCRIPTS_AVAILABLE_OPTIONS "${RENDU_SCRIPTS}" RENDU_SCRIPTS_INDEX)
    if (${RENDU_SCRIPTS_INDEX} EQUAL -1)
        rendu_log_fatal("RENDU_SCRIPTS 变量值 (${RENDU_SCRIPTS}) 非法！允许值为: ${RENDU_SCRIPTS_AVAILABLE_OPTIONS}，如有疑问请查阅论坛 TCE00019。")
    endif ()
endif ()

# ====================================================================
# 脚本模块配置
# 说明: 当 RENDU_SCRIPTS="custom" 时，为每个脚本模块创建独立的配置选项
# ====================================================================
include(${CMAKE_SOURCE_DIR}/cmake/macros/ConfigureScripts.cmake)
# 当 RENDU_SCRIPTS="custom" 时，构建所有脚本模块
rendu_get_script_module_list(RENDU_SCRIPT_MODULE_LIST)
foreach (SCRIPT_MODULE ${RENDU_SCRIPT_MODULE_LIST})
    rendu_script_module_name_to_variable(${SCRIPT_MODULE} RENDU_SCRIPT_MODULE_VARIABLE)
    set(${RENDU_SCRIPT_MODULE_VARIABLE} "default" CACHE STRING "${SCRIPT_MODULE} 模块的构建类型")
    set_property(CACHE ${RENDU_SCRIPT_MODULE_VARIABLE} PROPERTY STRINGS default disabled static dynamic)
endforeach ()

# ====================================================================
# 预编译头选项
# ====================================================================

option(RENDU_USE_SCRIPTPCH "编译脚本时使用预编译头" ON)
option(RENDU_USE_COREPCH "编译核心库时使用预编译头" ON)

# ====================================================================
# 动态链接配置
# 说明: 启用后所有库将以动态链接方式构建（实验性功能）
# ====================================================================
option(RENDU_WITH_DYNAMIC_LINKING "启用动态库链接" OFF)
rendu_is_dynamic_linking_required(RENDU_WITH_DYNAMIC_LINKING_FORCED)
if (RENDU_WITH_DYNAMIC_LINKING AND RENDU_WITH_DYNAMIC_LINKING_FORCED)
    set(RENDU_WITH_DYNAMIC_LINKING_FORCED OFF)
endif ()
if (RENDU_WITH_DYNAMIC_LINKING OR RENDU_WITH_DYNAMIC_LINKING_FORCED)
    set(BUILD_SHARED_LIBS ON)
else ()
    set(BUILD_SHARED_LIBS OFF)
endif ()

# ====================================================================
# 警告与调试选项
# ====================================================================
option(RENDU_WITH_WARNINGS "编译时显示所有警告" OFF)
option(RENDU_WITH_WARNINGS_AS_ERRORS "将警告视为错误" OFF)
option(RENDU_WITH_COREDEBUG "包含核心的额外调试代码" OFF)

# ====================================================================
# 指标与配置文件选项
# ====================================================================
option(RENDU_WITHOUT_METRICS "禁用指标上报（如 InfluxDB 和 Grafana）" OFF)
option(RENDU_WITH_DETAILED_METRICS "启用详细指标上报（如统计每个会话的更新时间）" OFF)
option(RENDU_COPY_CONF "将 authserver 和 worldserver 的 .conf.dist 文件复制到项目目录" ON)

# ====================================================================
# IDE 配置选项
# 说明: 控制源文件在 IDE 中的组织方式
#   - no: 不分组，所有源文件在同一层级
#   - flat: 按文件类型分组（如 Source Files、Header Files）
#   - hierarchical: 按目录结构分组（树形）
#   - hierarchical-folders: 按目录结构分组并显示文件夹
# ====================================================================
set(RENDU_WITH_SOURCE_TREE "hierarchical" CACHE STRING "为 IDE 构建源码树结构")
set_property(CACHE RENDU_WITH_SOURCE_TREE PROPERTY STRINGS no flat hierarchical hierarchical-folders)

# ====================================================================
# GIT 配置选项
# 说明: 禁用后将失去官方支持，无法自动获取版本信息
# ====================================================================
# GIT 相关选项
option(RENDU_WITHOUT_GIT "禁用 GIT 测试流程" OFF)

# ====================================================================
# 平台特定选项
# ====================================================================
# UNIX 平台专用选项
if (UNIX)
    option(RENDU_USE_LD_GOLD "使用 GNU gold 链接器" OFF)
endif ()