# ====================================================================
# 模块: RenduCheckPlatform
# 描述: 平台与编译器检测脚本
# 依赖模块:
#   - RenduLogging (日志)
# ====================================================================

# ====================================================================
# 函数: rendu_check_platform
# 描述: 检测平台位数和处理器架构
#
# 用法示例:
#   rendu_check_platform()
# ====================================================================
function(rendu_check_platform)
    # 检测 64 位或 32 位平台
    if (CMAKE_SIZEOF_VOID_P MATCHES 8)
        set(RENDU_PLATFORM 64)
    else ()
        set(RENDU_PLATFORM 32)
    endif ()
    set(RENDU_PLATFORM ${RENDU_PLATFORM} PARENT_SCOPE)
    rendu_log_info("检测到 ${RENDU_PLATFORM}-bit 平台")

    # 检测处理器架构
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "amd64|x86_64|AMD64")
        set(RENDU_SYSTEM_PROCESSOR "amd64")
    elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|ARM|aarch)64$")
        set(RENDU_SYSTEM_PROCESSOR "arm64")
    elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|ARM)$")
        set(RENDU_SYSTEM_PROCESSOR "arm")
    else ()
        set(RENDU_SYSTEM_PROCESSOR "x86")
    endif ()

# ====================================================================
# 兼容 MSVC 的 -A 平台参数
# 说明: 当使用 cmake -A x64 等参数时，覆盖自动检测的处理器架构
# ====================================================================
    if (CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        set(RENDU_SYSTEM_PROCESSOR "x86")
    elseif (CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
        set(RENDU_SYSTEM_PROCESSOR "amd64")
    elseif (CMAKE_GENERATOR_PLATFORM STREQUAL "ARM")
        set(RENDU_SYSTEM_PROCESSOR "arm")
    elseif (CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64")
        set(RENDU_SYSTEM_PROCESSOR "arm64")
    endif ()
    set(RENDU_SYSTEM_PROCESSOR ${RENDU_SYSTEM_PROCESSOR} PARENT_SCOPE)
    rendu_log_info("检测到 ${RENDU_SYSTEM_PROCESSOR} 处理器架构")

# ====================================================================
# 平台相关设置（仅在配置文件存在时加载）
# 说明: 加载平台特定的编译器和链接器设置
# ====================================================================
    if (WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/cmake/platform/win/settings.cmake")
        include("${CMAKE_SOURCE_DIR}/cmake/platform/win/settings.cmake")
    elseif (UNIX AND EXISTS "${CMAKE_SOURCE_DIR}/cmake/platform/unix/settings.cmake")
        include("${CMAKE_SOURCE_DIR}/cmake/platform/unix/settings.cmake")
    endif ()
endfunction()

# ====================================================================
# 函数: rendu_check_compiler
# 描述: 检测编译器类型并加载对应配置
#
# 用法示例:
#   rendu_check_compiler()
# ====================================================================
function(rendu_check_compiler)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/compiler/msvc/settings.cmake")
            include("${CMAKE_SOURCE_DIR}/cmake/compiler/msvc/settings.cmake")
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/compiler/clang/settings.cmake")
            include("${CMAKE_SOURCE_DIR}/cmake/compiler/clang/settings.cmake")
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/compiler/gcc/settings.cmake")
            include("${CMAKE_SOURCE_DIR}/cmake/compiler/gcc/settings.cmake")
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/compiler/icc/settings.cmake")
            include("${CMAKE_SOURCE_DIR}/cmake/compiler/icc/settings.cmake")
        endif ()
    endif ()
endfunction()

# ====================================================================
# 自动执行平台和编译器检测
# ====================================================================
rendu_check_platform()
rendu_check_compiler()