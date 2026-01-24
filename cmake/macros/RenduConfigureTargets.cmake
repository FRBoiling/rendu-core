# ====================================================================
# 模块: RenduConfigureTargets
# 描述: 定义项目的基础接口目标，用于统一配置编译选项和特性
# 依赖模块:
#   - 无
#
# 架构概述:
#   1. rendu-compile-option-interface  - 基础编译选项接口
#   2. rendu-feature-interface          - 项目特性接口
#   3. rendu-warning-interface          - 警告级别接口
#   4. rendu-default-interface          - 默认配置接口
#   5. rendu-no-warning-interface       - 静默警告接口
#   6. rendu-hidden-symbols-interface   - 符号隐藏接口
#   7. rendu-dependency-interface       - 依赖项专用接口
#   8. rendu-core-interface            - 核心模块接口
#
# 使用说明:
#   - 内部模块: PUBLIC rendu-core-interface
#   - 第三方依赖: PUBLIC rendu-dependency-interface
# ====================================================================

# ====================================================================
# 1. 核心编译选项配置接口
#   - 设置 C++20 标准
#   - 禁用扩展 (使用 -std=c++20 而非 -std=gnu++20)
#   - 定义构建类型和调试信息宏
# 说明: 所有目标应链接此接口以获得标准编译配置
# ====================================================================
add_library(rendu-compile-option-interface INTERFACE)
set(CMAKE_CXX_EXTENSIONS OFF)      # 使用 -std=c++20 而非 -std=gnu++20
set(CMAKE_CXX_STANDARD 20)
target_compile_definitions(rendu-compile-option-interface
    INTERFACE
    RENDU_BUILD_TYPE="$<CONFIG>"
    RENDU_BUILD_HAS_DEBUG_INFO=$<CONFIG:Debug,RelWithDebInfo>
)

# ====================================================================
# 2. 项目特性接口库
#   - 用于集中管理项目级特性宏和配置开关
#   - 示例: target_compile_definitions(rendu-feature-interface INTERFACE ENABLE_FEATURE)
# 说明: 可用于定义项目特定的功能开关，如 ENABLE_LOGGING、ENABLE_PROFILING 等
# ====================================================================
add_library(rendu-feature-interface INTERFACE)
# 预留接口: 用于集中管理项目级特性宏和配置开关
# 示例: target_compile_definitions(rendu-feature-interface INTERFACE ENABLE_FEATURE)

# ====================================================================
# 3. 警告级别接口库
#   - 平台相关配置由具体平台的 CMake 脚本设置编译器特定警告选项
# 说明: 警告级别在平台特定的 settings.cmake 中配置（如 compiler/gcc/settings.cmake）
# ====================================================================
add_library(rendu-warning-interface INTERFACE)
# 平台相关配置由具体平台的 CMake 脚本设置编译器特定警告选项

# ====================================================================
# 4. 默认接口库 (核心聚合接口)
#   - 聚合基础编译选项和项目特性接口
# 说明: 内部模块应链接此接口以获得完整的默认配置
# ====================================================================
add_library(rendu-default-interface INTERFACE)
target_link_libraries(rendu-default-interface
    INTERFACE
    rendu-compile-option-interface
    rendu-feature-interface
)

# ====================================================================
# 5. 静默警告接口库
#   - 禁用所有警告输出
# 说明: 用于第三方依赖库，避免其警告干扰项目构建
# ====================================================================
add_library(rendu-no-warning-interface INTERFACE)
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(rendu-no-warning-interface INTERFACE /W0)
else ()
    target_compile_options(rendu-no-warning-interface INTERFACE -w)
endif ()

# ====================================================================
# 6. 符号可见性控制接口
#   - 默认隐藏符号配置 (避免暴露内部实现细节)
#   - 示例: target_compile_options(rendu-hidden-symbols-interface INTERFACE -fvisibility=hidden)
# 说明: 用于控制动态库的符号可见性，减少二进制大小并提高安全性
# ====================================================================
add_library(rendu-hidden-symbols-interface INTERFACE)
# 默认隐藏符号配置 (避免暴露内部实现细节)
# 示例: target_compile_options(rendu-hidden-symbols-interface INTERFACE -fvisibility=hidden)

# ====================================================================
# 7. 依赖项接口库
#   - 聚合默认配置、静默警告和符号隐藏接口
#   - 用于第三方依赖
# 说明: 第三方库应链接此接口以获得一致的构建环境
# ====================================================================
add_library(rendu-dependency-interface INTERFACE)
target_link_libraries(rendu-dependency-interface
    INTERFACE
    rendu-default-interface
    rendu-no-warning-interface
    rendu-hidden-symbols-interface
)

# ====================================================================
# 8. 核心模块接口库
#   - 聚合默认配置和警告级别接口
#   - 用于内部模块
# 说明: 项目核心模块应链接此接口以获得完整的内部构建配置
# ====================================================================
add_library(rendu-core-interface INTERFACE)
target_link_libraries(rendu-core-interface
    INTERFACE
    rendu-default-interface
    rendu-warning-interface
)
