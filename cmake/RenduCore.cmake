# =============================================
# RenduCore CMake 统一入口模块
# =============================================
# 此模块按正确顺序加载所有 Rendu CMake 宏和功能
# 使用方法：在顶层 CMakeLists.txt 中 include(RenduCore)
# =============================================

# 1. 日志系统 (必须最先加载)
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduLogging.cmake)

# 初始化日志系统 (默认级别 INFO，前缀 "RD")
if (NOT DEFINED RENDU_CMAKE_LOG_LEVEL)
    rendu_set_log_level(INFO)
endif ()
if (NOT DEFINED RENDU_LOG_PREFIX)
    rendu_log_set_prefix("RD")
endif ()
rendu_log_info("正在加载 RenduCore CMake 模块...")

# 设置 CMAKE_CTEST_COMMAND 默认值 (如果未设置)
if (NOT DEFINED CMAKE_CTEST_COMMAND)
    find_program(CMAKE_CTEST_COMMAND NAMES ctest ctest.exe)
    if (NOT CMAKE_CTEST_COMMAND)
        rendu_log_warn("未找到 ctest 可执行文件，测试目标可能无法工作")
    else ()
        rendu_log_info("找到 ctest: ${CMAKE_CTEST_COMMAND}")
    endif ()
endif ()

# 2. CMake 策略和基础目标配置 (必须优先加载,为编译器设置提供目标)
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduCMakePolicies.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduCheckBuildDir.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduConfigureTargets.cmake)

# 3. 平台和编译器检测 (依赖 RenduConfigureTargets 提供的目标)
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduCheckPlatform.cmake)

# 4. 源文件和目录收集
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduCollectFiles.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduCollectDirectories.cmake)

# 5. 核心构建宏
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduAddExecutable.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduAddLibrary.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduAddTest.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduAddCustomTarget.cmake)

# 6. 项目组织宏
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduAddSubdirectories.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduSourceGroup.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduProject.cmake)

# 7. 高级功能 (按需加载)
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduPCHSupport.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/macros/ConfigureScripts.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/macros/EnsureVersion.cmake)

# 7. 扩展功能
include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduExtensions.cmake)

# 8. 打包和包管理（可选）
# include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduPackage.cmake)
# include(${CMAKE_CURRENT_LIST_DIR}/macros/RenduPackageManager.cmake)

rendu_log_info("RenduCore CMake 模块加载完成")
