# ====================================================================
# 模块: RenduProject
# 描述: 项目初始化和基础配置
# 依赖模块:
#   - RenduCMakePolicies (策略设置)
#   - RenduLogging (日志)
#   - RenduConfigureTargets (目标配置，由 RenduCore 统一加载)
#   - RenduPackageManager (包管理)
#
# 使用方法: 在项目根 CMakeLists.txt 中 include(RenduProject)
# ====================================================================

# ====================================================================
# 项目元信息
# ====================================================================
# 设置项目作者和许可证信息
set(PROJECT_AUTHOR "boil" CACHE STRING "Project author")
set(PROJECT_LICENSE "MIT" CACHE STRING "Project license")

# ====================================================================
# 构建安全设置
# ====================================================================
# 强制禁止源码内构建和源码修改
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)

# ====================================================================
# CMake 策略设置
# ====================================================================
# 设置 CMake 策略以获得更好的兼容性
include(RenduCMakePolicies)
rendu_set_policies()

# ====================================================================
# 默认构建类型配置
# ====================================================================
# 设置默认构建类型为 RelWithDebInfo
if (CMAKE_GENERATOR STREQUAL "Ninja Multi-Config")
    set(CMAKE_DEFAULT_BUILD_TYPE "RelWithDebInfo" CACHE INTERNAL "")
endif ()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type" FORCE)
endif ()

# ====================================================================
# 平台与基础目标配置
# ====================================================================
# 注意: RenduLogging 和 RenduConfigureTargets 已由 RenduCore 统一加载
# 这里只加载 RenduPackageManager
include(${CMAKE_SOURCE_DIR}/cmake/macros/RenduPackageManager.cmake)

# ====================================================================
# 常用模块与配置
# ====================================================================
# 加载项目常用的配置模块
include(${CMAKE_SOURCE_DIR}/cmake/options.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/genrev.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/showoptions.cmake)
