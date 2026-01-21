# ====================================================================
# 模块: RenduPCHSupport
# 描述: 预编译头支持工具函数
# 依赖模块:
#   - 无
# ====================================================================

# ====================================================================
# 函数: rendu_add_cxx_pch
# 描述: 为目标添加预编译头文件
#
# 参数:
#   TARGETS - 目标列表 (必填)
#   HEADER  - 预编译头文件路径 (必填)
#
# 用法示例:
#   rendu_add_library(
#       DIR ${CMAKE_CURRENT_SOURCE_DIR}
#       NAME mylib
#   )
#   rendu_add_cxx_pch(TARGETS mylib HEADER "${CMAKE_SOURCE_DIR}/src/precompiled.h")
# ====================================================================
function(rendu_add_cxx_pch)
    set(options "")
    set(oneValueArgs HEADER)
    set(multiValueArgs TARGETS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 检查必需参数
    if (NOT ARG_TARGETS OR NOT ARG_HEADER)
        message(FATAL_ERROR "rendu_add_cxx_pch: 必须指定 TARGETS 和 HEADER")
    endif ()

    foreach (target IN LISTS ARG_TARGETS)
        target_precompile_headers(${target} PRIVATE ${ARG_HEADER})
    endforeach ()

    rendu_log_info("为 ${ARG_TARGETS} 添加预编译头: ${ARG_HEADER}")
endfunction()

# ====================================================================
# 函数: rendu_reuse_cxx_pch
# 描述: 为目标复用已有目标的预编译头
#
# 参数:
#   TARGETS     - 目标列表 (必填)
#   REUSE_FROM  - 要复用的目标名称 (必填)
#
# 用法示例:
#   rendu_reuse_cxx_pch(TARGETS mylib2 REUSE_FROM mylib1)
# ====================================================================
function(rendu_reuse_cxx_pch)
    set(options "")
    set(oneValueArgs REUSE_FROM)
    set(multiValueArgs TARGETS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 检查必需参数
    if (NOT ARG_TARGETS OR NOT ARG_REUSE_FROM)
        message(FATAL_ERROR "rendu_reuse_cxx_pch: 必须指定 TARGETS 和 REUSE_FROM")
    endif ()

    foreach (target IN LISTS ARG_TARGETS)
        target_precompile_headers(${target} REUSE_FROM ${ARG_REUSE_FROM})
    endforeach ()

    rendu_log_info("${ARG_TARGETS} 复用 ${ARG_REUSE_FROM} 的预编译头")
endfunction()
