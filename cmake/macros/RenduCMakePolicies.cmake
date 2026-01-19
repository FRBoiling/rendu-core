# ====================================================================
# 模块: RenduCMakePolicies
# 描述: 设置项目编译所需的 CMake 策略
# 依赖模块:
#   - 无
# ====================================================================

# ====================================================================
# 宏: rendu_set_policies
# 描述: 设置一系列 CMake 策略以确保项目编译环境的一致性和正确性
#
# 策略说明:
#   CMP0144: find_dependency() 继承父项目的 REQUIRED/QUIET/NO_MODULE 等参数
#   CMP0153: find_dependency() 继承父项目的 NO_POLICY_SCOPE 参数
#   CMP0077: option() 命令影响缓存变量，而不强制缓存
#   CMP0126: set(... CACHE ...) 命令不缓存变量
#   CMP0135: 使用下载时间作为时间戳，便于 URL 变更时正确重建
#   CMP0150: 相对 git 仓库路径以父项目 remote 为基准
#   CMP0167: FindBoost 模块已被移除，使用 CONFIG 模式
#   CMP0174: 头文件单元测试支持
# ====================================================================
macro(rendu_set_policies)
    # CMP0144: 允许 find_dependency() 继承父项目的参数
    if (POLICY CMP0144)
        cmake_policy(SET CMP0144 NEW)
    endif ()

    # CMP0153: 允许 find_dependency() 继承父项目的 NO_POLICY_SCOPE 参数
    if (POLICY CMP0153)
        cmake_policy(SET CMP0153 NEW)
    endif ()

    # CMP0077: 允许 option() 命令影响缓存变量
    cmake_policy(SET CMP0077 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

    # CMP0126: 允许 set(... CACHE ...) 命令不缓存变量
    if (POLICY CMP0126)
        cmake_policy(SET CMP0126 NEW)
        set(CMAKE_POLICY_DEFAULT_CMP0126 NEW)
    endif ()

    # CMP0135: 使用下载时间作为时间戳
    if (POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
        set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
    endif ()

    # CMP0150: 相对 git 仓库路径以父项目 remote 为基准
    if (POLICY CMP0150)
        cmake_policy(SET CMP0150 NEW)
        set(CMAKE_POLICY_DEFAULT_CMP0150 NEW)
    endif ()

    # CMP0167: FindBoost 模块已被移除，使用 CONFIG 模式
    if (POLICY CMP0167)
        cmake_policy(SET CMP0167 NEW)
        set(CMAKE_POLICY_DEFAULT_CMP0167 NEW)
    endif ()

    # CMP0174: 头文件单元测试支持
    if (POLICY CMP0174)
        cmake_policy(SET CMP0174 NEW)
        set(CMAKE_POLICY_DEFAULT_CMP0174 NEW)
    endif ()
endmacro()
