# ====================================================================
# 模块: RenduPackageManager
# 描述: 包管理器,提供依赖包的查找、下载、缓存和安装功能
# 依赖模块:
#   - RenduLogging (日志)
#   - FetchContent (CMake 内置)
# ====================================================================

# ====================================================================
# 包管理器选项配置
# ====================================================================
# 包管理相关选项说明:
#   - RENDU_USE_LOCAL_PACKAGES: 总是尝试用 find_package 获取依赖
#   - RENDU_LOCAL_PACKAGES_ONLY: 只用 find_package 获取依赖
#   - RENDU_DOWNLOAD_SOURCE_ALL: 总是从源码下载依赖
#   - RENDU_DONT_UPDATE_MODULE_PATH: 不更新模块路径以允许 find_package
#   - RENDU_DONT_CREATE_PACKAGE_LOCK: 不在二进制路径创建包锁文件
#   - RENDU_INCLUDE_ALL_IN_PACKAGE_LOCK: 将所有通过 RenduPackageManager.cmake 添加的包加入包锁
#   - RENDU_USE_NAMED_CACHE_DIRECTORIES: 在缓存最深层使用包名目录
#   - RENDU_PACKAGE_LOCK_ENABLED: 包锁开启
# ====================================================================
option(RENDU_USE_LOCAL_PACKAGES "总是尝试用 find_package 获取依赖" OFF)
option(RENDU_LOCAL_PACKAGES_ONLY "只用 find_package 获取依赖" OFF)
option(RENDU_DOWNLOAD_SOURCE_ALL "总是从源码下载依赖" OFF)
option(RENDU_DONT_UPDATE_MODULE_PATH "不更新模块路径以允许 find_package" OFF)
option(RENDU_DONT_CREATE_PACKAGE_LOCK "不在二进制路径创建包锁文件" OFF)
option(RENDU_INCLUDE_ALL_IN_PACKAGE_LOCK "将所有通过 RenduPackageManager.cmake 添加的包加入包锁" OFF)
option(RENDU_USE_NAMED_CACHE_DIRECTORIES "在缓存最深层使用包名目录" OFF)
option(RENDU_PACKAGE_LOCK_ENABLED "包锁开启" OFF)

set(RENDU_CURRENT_LIST_FILE ${CMAKE_CURRENT_LIST_FILE} CACHE INTERNAL "")
set(RENDU_PACKAGES_LIST "" CACHE INTERNAL "")

# ====================================================================
# 缓存目录与模块路径配置
# ====================================================================
# ====================================================================
# 设置源码缓存目录和模块路径
# ====================================================================
set(RENDU_PACKAGES_CACHE "$ENV{HOME}/.rdpm/packages" CACHE PATH "用于下载 FetchContent 包的目录")

# ====================================================================
# 设置模块路径
# ====================================================================
if (NOT RENDU_DONT_UPDATE_MODULE_PATH AND NOT DEFINED CMAKE_FIND_PACKAGE_REDIRECTS_DIR)
    set(RENDU_MODULE_PATH "${CMAKE_BINARY_DIR}/rendu_modules" CACHE INTERNAL "")
# ====================================================================
# 移除旧模块（如果存在）
# ====================================================================
    if (EXISTS "${RENDU_MODULE_PATH}")
        file(REMOVE_RECURSE "${RENDU_MODULE_PATH}")
    endif ()
    file(MAKE_DIRECTORY "${RENDU_MODULE_PATH}")
# ====================================================================
# 本地包模块优先生效（确保 RENDU 模块优先于系统模块）
# ====================================================================
    set(CMAKE_MODULE_PATH "${RENDU_MODULE_PATH};${CMAKE_MODULE_PATH}")
endif ()

# ====================================================================
# 包锁文件配置
# ====================================================================
if (NOT RENDU_DONT_CREATE_PACKAGE_LOCK)
    set(RENDU_PACKAGE_LOCK_FILE "${CMAKE_BINARY_DIR}/rendu-package-lock.cmake" CACHE INTERNAL "")
    file(WRITE ${RENDU_PACKAGE_LOCK_FILE} "# RENDU 包锁文件\n# 此文件应提交到版本控制\n\n")
endif ()

include(FetchContent)

# ====================================================================
# FetchContent 基础目录配置
# ====================================================================
# 设置 FetchContent 的基础目录，支持用户自定义
if (DEFINED FETCHCONTENT_BASE_DIR)
    # 若用户设置了 FETCHCONTENT_BASE_DIR，则使用
    set(RENDU_FETCHCONTENT_BASE_DIR ${FETCHCONTENT_BASE_DIR})
    rendu_log_debug("使用用户设置的 FETCHCONTENT_BASE_DIR: ${RENDU_FETCHCONTENT_BASE_DIR}")
else ()
    set(RENDU_FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)
    rendu_log_debug("使用默认的 FETCHCONTENT_BASE_DIR: ${RENDU_FETCHCONTENT_BASE_DIR}")
endif ()

# ====================================================================
# 内部辅助函数 - Git URI 解析
# 描述: 尝试从 git 仓库 URI 推断包名
#
# 参数:
#   url          - git 仓库 uri
#   output_name  - 返回的包名变量名（通过 PARENT_SCOPE 返回）
#
# 返回:
#   output_name - 推断出的包名，未能推断则 unset
# ====================================================================
function(_get_package_name_from_git_uri url output_name)
    # 支持常见的 git 仓库 URI 结尾格式
    if ("${url}" MATCHES ".*/([^/]+)\\.git/?$")
        set(${output_name} "${CMAKE_MATCH_1}" PARENT_SCOPE)
        rendu_log_debug("从 git URI '${url}' 推断包名为 '${CMAKE_MATCH_1}'")
    else ()
        unset(${output_name} PARENT_SCOPE)
        rendu_log_debug("从 git URI '${url}' 未能推断出包名")
    endif ()
endfunction()

# ====================================================================
# 内部辅助函数 - URL 解析
# 描述: 从 URL 推断包名和版本
#
# 参数:
#   url            - 归档包的下载链接
#   output_name    - 返回的包名变量名（通过 PARENT_SCOPE 返回）
#   output_version - 返回的版本号变量名（通过 PARENT_SCOPE 返回）
#
# 返回:
#   output_name    - 推断出的包名，未能推断则 unset
#   output_version - 推断出的版本号，未能推断则 unset
# ====================================================================
function(_get_package_name_and_ver_from_url url output_name output_version)
    # 匹配常见归档文件名
    if ("${url}" MATCHES "[/\\?]([a-zA-Z0-9_\\.-]+)\\.(tar|tar\\.gz|tar\\.bz2|zip|ZIP)(\\?|/|$)")
        set(filename "${CMAKE_MATCH_1}")

        # 匹配 <name>-<version> 或 <name>_v<version> 格式
        if ("${filename}" MATCHES "^([a-zA-Z0-9_\\.-]+)[_-]v?(([0-9]+\\.)*[0-9]+[a-zA-Z0-9]*)$")
            set(${output_name} "${CMAKE_MATCH_1}" PARENT_SCOPE)
            set(${output_version} "${CMAKE_MATCH_2}" PARENT_SCOPE)
            # 匹配只有版本号的情况
        elseif ("${filename}" MATCHES "(([0-9]+\\.)+[0-9]+[a-zA-Z0-9]*)")
            unset(${output_name} PARENT_SCOPE)
            set(${output_version} "${CMAKE_MATCH_1}" PARENT_SCOPE)
        else ()
            # 仅有包名，无版本号
            set(${output_name} "${filename}" PARENT_SCOPE)
            unset(${output_version} PARENT_SCOPE)
        endif ()
    else ()
        # 非归档文件或无法识别
        unset(${output_name} PARENT_SCOPE)
        unset(${output_version} PARENT_SCOPE)
    endif ()
endfunction()

# ====================================================================
# 内部辅助函数 - 哈希处理
# 描述: 查找可用的最短哈希，减少路径长度
#
# 参数:
#   source_cache_dir  - 源码缓存目录
#   origin_hash       - 原始完整哈希
#   output_short_hash - 返回的最短哈希变量名（通过 PARENT_SCOPE 返回）
#
# 返回:
#   output_short_hash - 通过 PARENT_SCOPE 返回，未能找到则为 origin_hash
#
# 说明:
#   例如，如果 origin_hash 是 cccb77ae9609d2768ed80dd42cec54f77b1f1455，
#   会依次检查如下文件，直到找到一个为空或内容匹配 origin_hash:
#   * .../cccb.hash
#   * .../cccb77ae.hash
#   * .../cccb77ae9609.hash
#   * .../cccb77ae9609d276.hash
#   这样可以大概率用更短的路径，极少数情况下会检查更长的子串。
# ====================================================================
function(_get_shortest_hash source_cache_dir origin_hash output_short_hash)
    # 兼容旧版 RENDU 缓存，先检查是否存在完整哈希目录
    if (EXISTS "${source_cache_dir}/${origin_hash}")
        set(${output_short_hash} "${origin_hash}" PARENT_SCOPE)
        return()
    endif ()

    set(found_short_hash "")
    foreach (len RANGE 4 40 4)
        string(SUBSTRING "${origin_hash}" 0 ${len} short_hash)
        set(hash_lock "${source_cache_dir}/${short_hash}.lock")
        set(hash_fp "${source_cache_dir}/${short_hash}.hash")
        # 加锁，避免与其他 cmake 实例竞争
        file(LOCK "${hash_lock}" GUARD FUNCTION)

        # 确保哈希文件存在
        file(TOUCH "${hash_fp}")
        file(READ "${hash_fp}" hash_fp_contents)

        if (hash_fp_contents STREQUAL "")
            # 文件为空，写入原始哈希并选用该短哈希
            file(WRITE "${hash_fp}" "${origin_hash}")
            set(found_short_hash "${short_hash}")
            file(LOCK "${hash_lock}" RELEASE)
            break()
        elseif (hash_fp_contents STREQUAL "${origin_hash}")
            # 文件内容匹配，选用该短哈希
            set(found_short_hash "${short_hash}")
            file(LOCK "${hash_lock}" RELEASE)
            break()
        else ()
            # 文件内容不匹配，继续尝试更长的短哈希
            file(LOCK "${hash_lock}" RELEASE)
        endif ()
    endforeach ()

    # 若未找到合适短哈希，回退为完整哈希
    if (found_short_hash STREQUAL "")
        set(found_short_hash "${origin_hash}")
    endif ()

    set(${output_short_hash} "${found_short_hash}" PARENT_SCOPE)
endfunction()

# ====================================================================
# 内部辅助函数 - 本地包查找
# 描述: 查找本地包
#
# 参数:
#   resultVar  - 结果变量 (ON/OFF)，通过 PARENT_SCOPE 返回
#   pkgName    - 包名
#   pkgVersion - 期望的版本号
#   ARGN      - 额外传递给 find_package 的参数
#
# 返回:
#   resultVar - ON  找到本地包
#            OFF 未找到本地包
# ====================================================================
function(_find_package resultVar pkgName pkgVersion)
    string(REPLACE " " ";" extra_args "${ARGN}")
    find_package(${pkgName} ${pkgVersion} ${extra_args} QUIET)
    if (${pkgName}_FOUND)
        # 优先使用实际找到的包的版本
        set(foundVersion "${pkgVersion}")
        if (DEFINED ${pkgName}_VERSION)
            set(foundVersion "${${pkgName}_VERSION}")
        endif ()
        rendu_log_info(" 使用本地包 ${pkgName}@${foundVersion}")
        rendu_register_package(${pkgName} "${foundVersion}")
        set(${resultVar} ON PARENT_SCOPE)
    else ()
        set(${resultVar} OFF PARENT_SCOPE)
    endif ()
endfunction()

# ====================================================================
# 内部辅助函数 - 模块文件创建
# 描述: 为 RENDU 包创建自定义 FindXXX.cmake 模块，防止 find_package 找到系统库
#
# 参数:
#   pkgName - 包名
#   ARGN    - 可选附加内容（写入 FindXXX.cmake）
# ====================================================================
function(_create_module_file pkgName)
    if (NOT RENDU_DONT_UPDATE_MODULE_PATH)
        string(TOLOWER "${pkgName}" pkgNameLower)
        if (DEFINED CMAKE_FIND_PACKAGE_REDIRECTS_DIR)
            # CMake 3.24+ 支持的重定向机制，优先使用
            file(WRITE "${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}/${pkgNameLower}-config.cmake"
                    "include(\"\${CMAKE_CURRENT_LIST_DIR}/${pkgNameLower}-extra.cmake\" OPTIONAL)\n"
                    "include(\"\${CMAKE_CURRENT_LIST_DIR}/${pkgName}Extra.cmake\" OPTIONAL)\n"
            )
            file(WRITE "${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}/${pkgNameLower}-config-version.cmake"
                    "set(PACKAGE_VERSION_COMPATIBLE TRUE)\n"
                    "set(PACKAGE_VERSION_EXACT TRUE)\n"
            )
        else ()
            # 传统 FindXXX.cmake 方式
            file(WRITE "${RENDU_MODULE_PATH}/Find${pkgName}.cmake"
                    "include(\"${RENDU_CURRENT_LIST_FILE}\")\n${ARGN}\nset(${pkgName}_FOUND TRUE)\n"
            )
        endif ()
    endif ()
endfunction()

# ====================================================================
# 内部辅助函数 - 包状态检查
# 描述: 检查包是否已添加
#
# 参数:
#   pkgName    - 包名
#   pkgVersion - 期望的版本号 (可选)
#   resultVar  - 结果变量 (ON/OFF)，通过 PARENT_SCOPE 返回
#
# 返回:
#   resultVar - ON  包已添加且版本兼容
#            OFF 包未添加或版本不兼容
# ====================================================================
function(_check_if_package_already_added pkgName pkgVersion resultVar)
    if ("${pkgName}" IN_LIST RENDU_PACKAGES_LIST)
        rendu_get_package_version(${pkgName} currentVersion)
        if ("${currentVersion}" VERSION_LESS "${pkgVersion}")
            rendu_log_warn("需要更高版本的 ${pkgName} (${pkgVersion})，当前已包含版本为 (${currentVersion})。")
        endif ()
        _get_fetch_properties(${pkgName} sourceDir binaryDir)
        set(${pkgName}_ADDED OFF)
        set(${resultVar} ON PARENT_SCOPE)
        _export_variables(${pkgName})
    else ()
        set(${resultVar} OFF PARENT_SCOPE)
    endif ()
endfunction()

# ====================================================================
# 内部辅助函数 - 参数解析
# 描述: 解析 rendu_add_package 的单参数写法
#   例如 github:foo/bar@1.2.3 会被转换为 GITHUB_REPOSITORY;foo/bar;VERSION;1.2.3
#
# 参数:
#   arg     - 单参数字符串（如 github:foo/bar@1.2.3）
#   outArgs - 返回的解析结果变量名（通过 PARENT_SCOPE 返回）
#
# 返回:
#   outArgs - 解析后的参数列表字符串
# ====================================================================
function(_parse_add_package_single_arg arg outArgs)
    # 查找协议头
    if ("${arg}" MATCHES "^([a-zA-Z]+):(.+)$")
        string(TOLOWER "${CMAKE_MATCH_1}" scheme)
        set(uri "${CMAKE_MATCH_2}")

        # 检查 rendu 专用协议
        if (scheme STREQUAL "github")
            set(out "GITHUB_REPOSITORY;${uri}")
            set(packageType "git")
        elseif (scheme STREQUAL "gitlab")
            set(out "GITLAB_REPOSITORY;${uri}")
            set(packageType "git")
        elseif (scheme STREQUAL "gitee")
            set(out "GITEE_REPOSITORY;${uri}")
            set(packageType "git")
        elseif (scheme STREQUAL "bitbucket")
            set(out "BITBUCKET_REPOSITORY;${uri}")
            set(packageType "git")
            # 未找到 rendu 专用协议，尝试判断是否为通用 URL
        elseif (arg MATCHES ".git/?(@|#|$)")
            set(out "GIT_REPOSITORY;${arg}")
            set(packageType "git")
        else ()
            # 回退为 URL
            set(out "URL;${arg}")
            set(packageType "archive")
            # 也可以检测 SVN，但现在很少见，不做处理
            rendu_log_warn("未识别的协议 '${scheme}'，假定为 URL")
        endif ()
    else ()
        if (arg MATCHES ".git/?(@|#|$)")
            set(out "GIT_REPOSITORY;${arg}")
            set(packageType "git")
        else ()
            # 无法判断包类型
            rendu_log_error("无法识别包类型 '${arg}'")
        endif ()
    endif ()

    # 对所有包，@... 视为版本号，仅替换最后一个
    string(REGEX REPLACE "@([^@]+)$" ";VERSION;\\1" out "${out}")

    # 按包类型解析剩余部分
    if (packageType STREQUAL "git")
        # 对于 git 仓库，#... 视为 tag、分支或提交哈希
        string(REGEX REPLACE "#([^#]+)$" ";GIT_TAG;\\1" out "${out}")
    elseif (packageType STREQUAL "archive")
        # 对于归档包，#... 视为 URL 哈希
        string(REGEX REPLACE "#([^#]+)$" ";URL_HASH;\\1" out "${out}")
        # 如果未显式提供版本号，不做解析，后续由 _get_version_from_url 处理
    else ()
        # 理论上不会到这里，若到此说明上面代码有问题
        rendu_log_error("不支持的包类型 '${packageType}'，参数为 '${arg}'")
    endif ()

    set(${outArgs} ${out} PARENT_SCOPE)
endfunction()

# ====================================================================
# 内部辅助函数 - Git 状态检查
# 描述: 检查 git 仓库工作区是否干净
#
# 参数:
#   repo_path - 仓库路径
#   git_tag   - 检查的 tag/分支/commit
#   is_clean  - 返回结果变量名（通过 PARENT_SCOPE 返回，TRUE=干净，FALSE=有更改）
#
# 返回:
#   is_clean - TRUE 表示干净，FALSE 表示有未提交或未推送的更改
# ====================================================================
function(_check_git_working_dir_is_clean repo_path git_tag is_clean)
    # find_package(Git REQUIRED)

    if (NOT GIT_EXECUTABLE)
        # 没有 git 可执行文件，假定目录是干净的
        set(${is_clean} TRUE PARENT_SCOPE)
        return()
    endif ()

    # 检查未提交的更改
    execute_process(
            COMMAND "${GIT_EXECUTABLE}" status --porcelain
            RESULT_VARIABLE result_git_status
            OUTPUT_VARIABLE repo_status
            OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
            WORKING_DIRECTORY "${repo_path}"
    )
    if (result_git_status)
        # 理论上不会发生，假定干净
        rendu_log_warn("调用 git status 检查 ${repo_path} 失败")
        set(${is_clean} TRUE PARENT_SCOPE)
        return()
    endif ()

    if (NOT "${repo_status}" STREQUAL "")
        set(${is_clean} FALSE PARENT_SCOPE)
        return()
    endif ()

    # 检查已提交的更改（与指定 tag/分支/commit 的 diff）
    execute_process(
            COMMAND "${GIT_EXECUTABLE}" diff -s --exit-code "${git_tag}"
            RESULT_VARIABLE result_git_diff
            OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_QUIET
            WORKING_DIRECTORY "${repo_path}"
    )

    if (result_git_diff EQUAL 0)
        set(${is_clean} TRUE PARENT_SCOPE)
    else ()
        set(${is_clean} FALSE PARENT_SCOPE)
    endif ()
endfunction()

# ====================================================================
# 内部辅助函数 - 补丁处理
# 描述: 为 ExternalProject_Add() 添加 PATCH_COMMAND
#
# 该函数接收 ARGN 中的补丁文件列表，并生成相应的 PATCH_COMMAND
#
# 参数:
#   ARGN - 补丁文件列表
# ====================================================================
function(_add_patches)
    # 如果未提供补丁文件，则直接返回
    if (NOT ARGN)
        return()
    endif ()

    # 查找 patch 程序
    find_program(PATCH_EXECUTABLE patch)
    if (CMAKE_HOST_WIN32 AND NOT PATCH_EXECUTABLE)
        # Windows 下尝试从 Git 安装路径查找 patch.exe
        find_package(Git QUIET)
        if (GIT_EXECUTABLE)
            get_filename_component(git_dir ${GIT_EXECUTABLE} DIRECTORY)
            get_filename_component(git_parent_dir ${git_dir} DIRECTORY)
            get_filename_component(git_grandparent_dir ${git_parent_dir} DIRECTORY)
            find_program(PATCH_EXECUTABLE patch HINTS "${git_parent_dir}/usr/bin" "${git_grandparent_dir}/usr/bin")
        endif ()
    endif ()

    # 如果未找到 patch 可执行文件，则记录错误并终止函数
    if (NOT PATCH_EXECUTABLE)
        rendu_log_fatal(FATAL_ERROR "未找到 patch 可执行文件，无法使用 PATCHES 关键字。")
    endif ()

    # 初始化 PATCH_COMMAND 列表
    set(patch_commands)

    # 遍历补丁文件列表，确保每个补丁文件存在，并构建 PATCH_COMMAND
    foreach (PATCH_FILE ${ARGN})
        # 检查补丁文件是否存在
        if (NOT EXISTS "${PATCH_FILE}")
            if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/${PATCH_FILE}")
                rendu_log_fatal(FATAL_ERROR "补丁文件 '${PATCH_FILE}' 不存在。")
            endif ()
            set(PATCH_FILE "${CMAKE_CURRENT_LIST_DIR}/${PATCH_FILE}")
        endif ()

        # 获取补丁文件的绝对路径
        get_filename_component(PATCH_FILE "${PATCH_FILE}" ABSOLUTE)

        # 构造 patch 命令并添加到 PATCH_COMMAND 列表中
        list(APPEND patch_commands "${PATCH_EXECUTABLE}" "-p1" "<" "${PATCH_FILE}")
    endforeach ()

    # 如果存在补丁命令，则构建 PATCH_COMMAND 字符串
    if (patch_commands)
        list(JOIN patch_commands " && " patch_command_string)
        set(patch_command_string "PATCH_COMMAND ${patch_command_string}")
    endif ()

    # 将 PATCH_COMMAND 字符串添加到 ARGS_UNPARSED_ARGUMENTS 中，并传递到父作用域
    set(ARGS_UNPARSED_ARGUMENTS "${ARGS_UNPARSED_ARGUMENTS} ${patch_command_string}" PARENT_SCOPE)
endfunction()

# ====================================================================
# 内部辅助函数 - FetchContent 重载
# 描述: 重载 FetchContent 内部属性，允许 RenduPackageManager.cmake 重载 FetchContent 调用
#
# 参数:
#   contentName  - 包名
#   SOURCE_DIR   - 源码目录（命名参数）
#   BINARY_DIR   - 二进制目录（命名参数）
#
# 返回:
#   无（直接设置全局属性）
#
# 说明:
#   这些是 CMake 内部属性，使用时需谨慎，未来 CMake 版本可能需要调整
#   来源: https://github.com/Kitware/CMake/blob/dc3d0b5a0a7d26d43d6cfeb511e224533b5d188f/Modules/FetchContent.cmake#L1152
# ====================================================================
function(_override_fetchcontent contentName)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "SOURCE_DIR;BINARY_DIR" "")
    if (NOT "${arg_UNPARSED_ARGUMENTS}" STREQUAL "")
        rendu_log_error("不支持的参数: ${arg_UNPARSED_ARGUMENTS}")
    endif ()

    string(TOLOWER ${contentName} contentNameLower)
    set(prefix "_FetchContent_${contentNameLower}")

    set(propertyName "${prefix}_sourceDir")
    define_property(
            GLOBAL
            PROPERTY ${propertyName}
            BRIEF_DOCS "FetchContent_Populate() 的内部实现细节"
            FULL_DOCS "FetchContent_Populate() 用于 ${contentName} 的详细信息"
    )
    set_property(GLOBAL PROPERTY ${propertyName} "${arg_SOURCE_DIR}")

    set(propertyName "${prefix}_binaryDir")
    define_property(
            GLOBAL
            PROPERTY ${propertyName}
            BRIEF_DOCS "FetchContent_Populate() 的内部实现细节"
            FULL_DOCS "FetchContent_Populate() 用于 ${contentName} 的详细信息"
    )
    set_property(GLOBAL PROPERTY ${propertyName} "${arg_BINARY_DIR}")

    set(propertyName "${prefix}_populated")
    define_property(
            GLOBAL
            PROPERTY ${propertyName}
            BRIEF_DOCS "FetchContent_Populate() 的内部实现细节"
            FULL_DOCS "FetchContent_Populate() 用于 ${contentName} 的详细信息"
    )
    set_property(GLOBAL PROPERTY ${propertyName} TRUE)
endfunction()

# ====================================================================
# 内部辅助函数 - 变量导出
# 描述: 导出变量到父作用域
#
# 参数:
#   varName - 要导出的变量名前缀
# ====================================================================
macro(_export_variables varName)
    set(${varName}_SOURCE_DIR "${${varName}_SOURCE_DIR}" PARENT_SCOPE)
    set(${varName}_BINARY_DIR "${${varName}_BINARY_DIR}" PARENT_SCOPE)
    set(${varName}_ADDED "${${varName}_ADDED}" PARENT_SCOPE)
endmacro()

# ====================================================================
# 内部辅助函数 - 包锁文件管理
# 描述: 向 rendu-package-lock.cmake 文件中添加一个新的包
#
# 参数:
#   pkgName - 要添加到 rendu-package-lock.cmake 文件中的包名
#   ARGN    - 传递给 rendu_declare_package 的额外参数（可选）
# ====================================================================
function(_add_to_package_lock pkgName)
    # 检查是否禁止创建 package.lock 文件
    if (NOT RENDU_DONT_CREATE_PACKAGE_LOCK)
        # 格式化传递给 rendu_declare_package 的参数
        _format_package_arguments(formatted_args false ${ARGN})

        # 构造注释字符串
        set(comment_string "# ${pkgName}\n")
        set(comment_string "${comment_string}# rendu_declare_package(${pkgName}\n${formatted_args}#)\n")

        # 将包信息追加到 rendu-package-lock 文件中
        file(APPEND ${RENDU_PACKAGE_LOCK_FILE} "${comment_string}")
    endif ()
endfunction()

# ====================================================================
# 内部辅助函数 - 包锁注释管理
# 描述: 向 rendu-package-lock.cmake 文件中添加关于包的注释
#
# 参数:
#   pkgName - 要添加注释的包名
#   ARGN    - 传递给 rendu_declare_package 的额外参数（可选）
# ====================================================================
function(_add_comment_to_package_lock pkgName)
    # 检查是否禁止创建 rendu-package-lock.cmake 文件
    if (NOT RENDU_DONT_CREATE_PACKAGE_LOCK)
        # 格式化传递给 rendu_declare_package 的参数，并添加注释标记
        _format_package_arguments(formatted_args true ${ARGN})

        # 构造注释字符串
        set(comment_string "# ${pkgName} (未指定版本)\n")
        set(comment_string "${comment_string}# rendu_declare_package(${pkgName}\n${formatted_args}#)\n")

        # 将注释追加到 rendu-package-lock.cmake 文件中
        file(APPEND ${RENDU_PACKAGE_LOCK_FILE} "${comment_string}")
    endif ()
endfunction()

# ====================================================================
# 内部辅助函数 - 包属性存储
# 描述: 存储和缓存包的源目录和二进制目录属性
#
# 参数:
#   pkgName    - 包名，用于构造缓存变量的名称
#   source_dir - 包的源目录路径
#   binary_dir - 包的二进制目录路径
# ====================================================================
function(_store_fetch_properties pkgName source_dir binary_dir)
    # 缓存包的源目录路径
    set(RENDU_PACKAGE_${pkgName}_SOURCE_DIR "${source_dir}" CACHE INTERNAL "")
    # 缓存包的二进制目录路径
    set(RENDU_PACKAGE_${pkgName}_BINARY_DIR "${binary_dir}" CACHE INTERNAL "")
endfunction()

# ====================================================================
# 内部辅助函数 - 子目录添加
# 描述: 按选项作为子目录添加包
#
# 参数:
#   pkgName       - 包名
#   download_only - 是否仅下载（ON/OFF）
#   source_dir    - 源码目录
#   binary_dir    - 二进制目录
#   exclude       - 是否 EXCLUDE_FROM_ALL（ON/OFF）
#   system        - 是否 SYSTEM（ON/OFF，CMake >= 3.25 支持）
#   options       - 额外的 CMake 选项列表
# ====================================================================
function(_add_subdirectory pkgName download_only source_dir binary_dir exclude system options)
    if (NOT download_only AND EXISTS "${source_dir}/CMakeLists.txt")
        set(add_subdirectory_extra_args "")
        if (exclude)
            list(APPEND add_subdirectory_extra_args EXCLUDE_FROM_ALL)
        endif ()
        if (system AND "${CMAKE_VERSION}" VERSION_GREATER_EQUAL "3.25")
            # SYSTEM 属性仅 CMake 3.25+ 支持
            list(APPEND add_subdirectory_extra_args SYSTEM)
        endif ()
        # rendu_log_set_prefix(${pkgName}:)
        add_subdirectory("${source_dir}" "${binary_dir}" ${add_subdirectory_extra_args})
        # rendu_log_rollback_prefix(${_old_indent}:)
    endif ()
endfunction()

# ====================================================================
# 内部辅助函数 - FetchContent 调用
# 描述: 通过 FetchContent 下载已声明包，并导出变量
#
# 参数:
#   pkgName       - 包名
#   download_only - 是否仅下载（ON/OFF）
#   populated    - 结果变量（ON/OFF），通过 PARENT_SCOPE 返回
#   ARGN         - 其余参数直接传递给 FetchContent_Populate
#
# 返回:
#   populated - ON  已下载
#            OFF 未下载
#
# 同时将 ${pkgName}_SOURCE_DIR 和 ${pkgName}_BINARY_DIR 导出到父作用域
# ====================================================================
function(rendu_fetch_content pkgName download_only populated)
    set(${populated} OFF PARENT_SCOPE)

    FetchContent_GetProperties(${pkgName})

    string(TOLOWER "${pkgName}" lower_case_name)

    if (NOT ${lower_case_name}_POPULATED)
        rendu_log_info("正在下载包: ${pkgName}...")
        if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.30.3")
            if (download_only)
                # 只下载依赖，不加入到构建
                FetchContent_Populate(
                        ${pkgName}
                        SOURCE_DIR "${RENDU_FETCHCONTENT_BASE_DIR}/${lower_case_name}-src"
                        BINARY_DIR "${RENDU_FETCHCONTENT_BASE_DIR}/${lower_case_name}-build"
                        SUBBUILD_DIR "${RENDU_FETCHCONTENT_BASE_DIR}/${lower_case_name}-subbuild"
                        ${ARGN}
                )
            else ()
                FetchContent_MakeAvailable(${pkgName})
            endif ()
        else ()
            FetchContent_Populate(${pkgName})
        endif ()
        rendu_log_info("包 ${pkgName} 下载完成")
        set(${populated} ON PARENT_SCOPE)
    else ()
        rendu_log_info("包 ${pkgName} 已存在缓存，跳过下载")
    endif ()

    # 存储源码和二进制目录属性
    _store_fetch_properties(
            ${pkgName} ${${lower_case_name}_SOURCE_DIR} ${${lower_case_name}_BINARY_DIR}
    )

    set(${pkgName}_SOURCE_DIR
            ${${lower_case_name}_SOURCE_DIR}
            PARENT_SCOPE
    )
    set(${pkgName}_BINARY_DIR
            ${${lower_case_name}_BINARY_DIR}
            PARENT_SCOPE
    )
endfunction()

# ====================================================================
# 内部辅助函数 - 选项解析
# 描述: 拆分包选项
#
# 参数:
#   option    - 形如 "KEY VALUE" 或 "KEY" 的字符串
#   key_var   - 返回的选项名变量名（通过 PARENT_SCOPE 返回）
#   value_var - 返回的选项值变量名（通过 PARENT_SCOPE 返回）
#
# 返回:
#   key_var   - 选项名
#   value_var - 选项值
# ====================================================================
function(_parse_option option key_var value_var)
    string(REGEX MATCH "^[^ ]+" option_key "${option}")
    string(LENGTH "${option}" option_length)
    string(LENGTH "${option_key}" option_key_length)
    if (option_key_length STREQUAL option_length)
        # 未指定值，默认设置为 "ON"
        set(option_value "ON")
    else ()
        math(EXPR value_start "${option_key_length}+1")
        string(SUBSTRING "${option}" "${value_start}" "-1" option_value)
        string(STRIP "${option_value}" option_value) # 去除前后空格
    endif ()
    set(${key_var} "${option_key}" PARENT_SCOPE)
    set(${value_var} "${option_value}" PARENT_SCOPE)
endfunction()

# ====================================================================
# 内部辅助函数 - 版本推断
# 描述: 从 git tag 猜测包版本
#
# 参数:
#   git_tag        - git 标签或哈希
#   output_version - 返回的版本号变量名（通过 PARENT_SCOPE 返回，哈希时为 0）
#
# 返回:
#   output_version - 解析出的版本号或 0
# ====================================================================
function(_get_version_from_git_tag git_tag output_version)
    string(LENGTH "${git_tag}" length)
    if (length EQUAL 40)
        # git_tag 很可能是 git 哈希
        set(${output_version} 0 PARENT_SCOPE)
    else ()
        string(REGEX MATCH "v?([0-9][0-9.]*).*" _ "${git_tag}")
        if (CMAKE_MATCH_1)
            set(${output_version} "${CMAKE_MATCH_1}" PARENT_SCOPE)
        else ()
            set(${output_version} 0 PARENT_SCOPE)
        endif ()
    endif ()
endfunction()

# ====================================================================
# 内部辅助函数 - Git 标签哈希判断
# 描述: 判断 git tag 是否为提交哈希
#
# 参数:
#   git_tag       - git 标签、分支名或哈希
#   output_result - 返回结果变量名（通过 PARENT_SCOPE 返回，1=哈希，0=非哈希）
#
# 返回:
#   output_result - 1 表示是哈希，0 表示不是
# ====================================================================
function(_is_git_tag_commit_hash git_tag output_result)
    string(LENGTH "${git_tag}" length)
    # 完整哈希为 40 位，短哈希至少 7 位
    if (length LESS 7 OR length GREATER 40)
        set(${output_result} 0 PARENT_SCOPE)
    else ()
        if ("${git_tag}" MATCHES "^[a-fA-F0-9]+$")
            set(${output_result} 1 PARENT_SCOPE)
        else ()
            set(${output_result} 0 PARENT_SCOPE)
        endif ()
    endif ()
endfunction()

# ====================================================================
# 内部辅助函数 - 参数格式化
# 描述: 美化包参数，生成格式化字符串（可用于包锁文件或注释）
#
# 参数:
#   out_var       - 返回的格式化字符串变量名（通过 PARENT_SCOPE 返回）
#   is_in_comment - 是否以注释形式输出（ON=加#，OFF=正常）
#   ARGN         - rendu 包参数列表
#
# 返回:
#   out_var - 格式化后的参数字符串
# ====================================================================
function(_format_package_arguments out_var is_in_comment)
    # 单值参数列表
    set(oneValueArgs
            NAME
            FORCE
            VERSION
            GIT_TAG
            DOWNLOAD_ONLY
            GITHUB_REPOSITORY
            GITLAB_REPOSITORY
            GITEE_REPOSITORY
            BITBUCKET_REPOSITORY
            GIT_REPOSITORY
            SOURCE_DIR
            FIND_PACKAGE_ARGUMENTS
            NO_CACHE
            SYSTEM
            GIT_SHALLOW
            EXCLUDE_FROM_ALL
            SOURCE_SUBDIR
    )
    # 多值参数列表
    set(multiValueArgs URL OPTIONS DOWNLOAD_COMMAND)
    # 解析参数
    cmake_parse_arguments(ARGS "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(PRETTY_OUT_VAR "")

    # 处理单值参数
    foreach (oneArgName ${oneValueArgs})
        if (DEFINED ARGS_${oneArgName})
            if (is_in_comment)
                string(APPEND PRETTY_OUT_VAR "#")
            endif ()
            if (${oneArgName} STREQUAL "SOURCE_DIR")
                # 将源码目录中的 CMAKE_SOURCE_DIR 替换为变量形式，便于移植
                string(REPLACE ${CMAKE_SOURCE_DIR} "\${CMAKE_SOURCE_DIR}" ARGS_${oneArgName} replaced_source_dir)
                string(APPEND PRETTY_OUT_VAR "  ${oneArgName} ${replaced_source_dir}\n")
            else ()
                string(APPEND PRETTY_OUT_VAR "  ${oneArgName} ${ARGS_${oneArgName}}\n")
            endif ()
        endif ()
    endforeach ()

    # 处理多值参数
    foreach (multiArgName ${multiValueArgs})
        if (DEFINED ARGS_${multiArgName})
            if (is_in_comment)
                string(APPEND PRETTY_OUT_VAR "#")
            endif ()
            string(APPEND PRETTY_OUT_VAR "  ${multiArgName}\n")
            foreach (singleOption ${ARGS_${multiArgName}})
                if (is_in_comment)
                    string(APPEND PRETTY_OUT_VAR "#")
                endif ()
                string(APPEND PRETTY_OUT_VAR "    \"${singleOption}\"\n")
            endforeach ()
        endif ()
    endforeach ()

    # 处理未解析参数
    if (NOT "${ARGS_UNPARSED_ARGUMENTS}" STREQUAL "")
        if (is_in_comment)
            string(APPEND PRETTY_OUT_VAR "#")
        endif ()
        string(APPEND PRETTY_OUT_VAR " ")
        foreach (ARGS_UNPARSED_ARGUMENT ${ARGS_UNPARSED_ARGUMENTS})
            string(APPEND PRETTY_OUT_VAR " ${ARGS_UNPARSED_ARGUMENT}")
        endforeach ()
        string(APPEND PRETTY_OUT_VAR "\n")
    endif ()

    # 返回结果
    set(${out_var}
            "${PRETTY_OUT_VAR}"
            PARENT_SCOPE
    )
endfunction()

# ====================================================================
# 内部辅助函数 - 包属性获取
# 描述: 获取已定义包的属性
#
# 参数:
#   pkgName         - 包名
#   source_dir_var  - 返回的源码目录变量名
#   binary_dir_var  - 返回的二进制目录变量名
# ====================================================================
function(_get_fetch_properties pkgName source_dir_var binary_dir_var)
    set(${source_dir_var}
            "${RENDU_PACKAGE_${pkgName}_SOURCE_DIR}"
            PARENT_SCOPE
    )
    set(${binary_dir_var}
            "${RENDU_PACKAGE_${pkgName}_BINARY_DIR}"
            PARENT_SCOPE
    )
endfunction()

# ====================================================================
# 公开 API - FetchContent 声明
# 描述: 用 FetchContent_Declare 声明包
#
# 参数:
#   pkgName - 包名
#   ARGN    - 其余参数直接传递给 FetchContent_Declare
# ====================================================================
function(rendu_fetchcontent_declare pkgName)
    if (NOT pkgName)
        rendu_log_fatal("rendu_fetchcontent_declare 未指定包名")
    endif ()
    FetchContent_Declare(${pkgName} ${ARGN})
endfunction()

# ====================================================================
# 公开 API - 包锁文件管理
# 描述: 如果包锁文件存在则包含，并创建 rendu-update-package-lock 目标用于更新
#
# 参数:
#   file - 包锁文件路径
# ====================================================================
macro(rendu_use_package_lock file)
    if (NOT RENDU_DONT_CREATE_PACKAGE_LOCK)
        get_filename_component(absolut_package_lock_path ${file} ABSOLUTE)
        if (EXISTS ${absolut})
            include(${absolut_package_lock_path})
        endif ()
        if (NOT TARGET rendu-update-package-lock)
            add_custom_target(rendu-update-package-lock COMMAND ${CMAKE_COMMAND} -E copy ${RENDU_PACKAGE_LOCK_FILE} ${absolut_package_lock_path})
        endif ()
        set(RENDU_PACKAGE_LOCK_ENABLED ON)
    endif ()
endmacro()

# ====================================================================
# 公开 API - 包注册与查询
# ====================================================================

# ====================================================================
# 注册已添加包
# 描述: 注册已添加包到全局包列表
#
# 参数:
#   pkgName    - 包名
#   pkgVersion - 包版本
# ====================================================================
function(rendu_register_package pkgName pkgVersion)
    list(APPEND RENDU_PACKAGES_LIST ${pkgName})
    set(RENDU_PACKAGES_LIST
            ${RENDU_PACKAGES_LIST}
            CACHE INTERNAL ""
    )
    set("RENDU_PACKAGE_${pkgName}_VERSION"
            ${pkgVersion}
            CACHE INTERNAL ""
    )
endfunction()

# ====================================================================
# 获取包当前版本
# 描述: 获取包当前版本
#
# 参数:
#   pkgName    - 包名
#   outVersion - 返回的版本变量名（通过 PARENT_SCOPE 返回）
# ====================================================================
function(rendu_get_package_version pkgName outVersion)
    set(${outVersion} "${RENDU_PACKAGE_${pkgName}_VERSION}" PARENT_SCOPE)
endfunction()

# ====================================================================
# 声明包
# 描述: 声明包，后续 rendu_add_package 会使用这些参数，之前的声明不会被覆盖
#
# 参数:
#   pkgName - 包名
#   ARGN    - 传递给 rendu_declare_package 的额外参数
# ====================================================================
macro(rendu_declare_package pkgName)
    if (NOT DEFINED "RENDU_DECLARATION_${pkgName}")
        set("RENDU_DECLARATION_${pkgName}" "${ARGN}")
    endif ()
endmacro()

# ====================================================================
# 公开 API - 包查找与添加
# ====================================================================

# ====================================================================
# 本地查找包，否则回退到 rendu_add_package
# 描述: 本地查找包，否则回退到 rendu_add_package
#
# 参数:
#   NAME                 - 包名
#   VERSION              - 包版本
#   GIT_TAG              - Git 标签
#   FIND_PACKAGE_ARGUMENTS - 传递给 find_package 的额外参数
# ====================================================================
function(rendu_find_package)
    set(oneValueArgs NAME VERSION GIT_TAG FIND_PACKAGE_ARGUMENTS)

    cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})

    if (NOT DEFINED ARG_VERSION)
        if (DEFINED ARG_GIT_TAG)
            _get_version_from_git_tag("${ARG_GIT_TAG}" ARG_VERSION)
        endif ()
    endif ()

    set(downloadPackage ${RENDU_DOWNLOAD_SOURCE_ALL})
    if (DEFINED RENDU_DOWNLOAD_${ARG_NAME})
        set(downloadPackage ${RENDU_DOWNLOAD_${ARG_NAME}})
    elseif (DEFINED ENV{RENDU_DOWNLOAD_${ARG_NAME}})
        set(downloadPackage $ENV{RENDU_DOWNLOAD_${ARG_NAME}})
    endif ()
    if (downloadPackage)
        rendu_add_package(${ARGN})
        _export_variables(${ARG_NAME})
        return()
    endif ()

    _find_package(find ${ARG_NAME} "${ARG_VERSION}" ${ARG_FIND_PACKAGE_ARGUMENTS})

    if (NOT find)
        rendu_add_package(${ARGN})
        _export_variables(${ARG_NAME})
    endif ()

endfunction()

# ====================================================================
# 从源码下载并添加包
# 描述: 从源码下载并添加包
#
# 参数:
#   NAME                      - 包名
#   FORCE                     - 强制重新下载
#   VERSION                   - 包版本
#   GIT_TAG                   - Git 标签
#   DOWNLOAD_ONLY             - 仅下载不构建
#   GITHUB_REPOSITORY         - GitHub 仓库地址
#   GITLAB_REPOSITORY         - GitLab 仓库地址
#   GITEE_REPOSITORY          - Gitee 仓库地址
#   BITBUCKET_REPOSITORY      - Bitbucket 仓库地址
#   GIT_REPOSITORY            - Git 仓库地址
#   SOURCE_DIR               - 源码目录
#   FIND_PACKAGE_ARGUMENTS    - 传递给 find_package 的额外参数
#   NO_CACHE                 - 不使用缓存
#   SYSTEM                   - 系统包
#   GIT_SHALLOW              - 浅克隆
#   GIT_SUBMODULES_RECURSIVE - 递归子模块
#   EXCLUDE_FROM_ALL         - 排除从所有目标
#   SOURCE_SUBDIR            - 源码子目录
#   CUSTOM_CACHE_KEY         - 自定义缓存键
#   URL                      - 下载 URL
#   OPTIONS                  - CMake 选项
#   DOWNLOAD_COMMAND         - 自定义下载命令
#   PATCHES                  - 补丁文件列表
# ====================================================================
function(rendu_add_package)
    rendu_set_policies()

    set(oneValueArgs
            NAME
            FORCE
            VERSION
            GIT_TAG
            DOWNLOAD_ONLY
            GITHUB_REPOSITORY
            GITLAB_REPOSITORY
            GITEE_REPOSITORY
            BITBUCKET_REPOSITORY
            GIT_REPOSITORY
            SOURCE_DIR
            FIND_PACKAGE_ARGUMENTS
            NO_CACHE
            SYSTEM
            GIT_SHALLOW
            GIT_SUBMODULES_RECURSIVE
            EXCLUDE_FROM_ALL
            SOURCE_SUBDIR
            CUSTOM_CACHE_KEY
    )

    set(multiValueArgs
            URL
            OPTIONS
            DOWNLOAD_COMMAND
            PATCHES
    )

    list(LENGTH ARGN argnLength)

    # 解析单参数简写
    if (argnLength EQUAL 1)
        _parse_add_package_single_arg("${ARGN}" ARGN)
        # 单参数简写默认 EXCLUDE_FROM_ALL 和 SYSTEM
        set(ARGN "${ARGN};EXCLUDE_FROM_ALL;YES;SYSTEM;YES;")
        # 解析 URI 简写参数
    elseif (argnLength GREATER 1 AND "${ARGV0}" STREQUAL "URI")
        list(REMOVE_AT ARGN 0 1) # 移除 "URI gh:<...>@version#tag"
        _parse_add_package_single_arg("${ARGV1}" ARGV0)
        set(ARGN "${ARGV0};EXCLUDE_FROM_ALL;YES;SYSTEM;YES;${ARGN}")
    endif ()

    cmake_parse_arguments(ARGS "" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    # 设置参数默认值
    if (NOT DEFINED ARGS_VERSION)
        if (DEFINED ARGS_GIT_TAG)
            _get_version_from_git_tag("${ARGS_GIT_TAG}" ARGS_VERSION)
        endif ()
    endif ()

    if (ARGS_DOWNLOAD_ONLY)
        set(download_only ${ARGS_DOWNLOAD_ONLY})
    else ()
        set(download_only NO)
    endif ()

    if (DEFINED ARGS_GITHUB_REPOSITORY)
        set(ARGS_GIT_REPOSITORY "https://github.com/${ARGS_GITHUB_REPOSITORY}.git")
    elseif (DEFINED ARGS_GITLAB_REPOSITORY)
        set(ARGS_GIT_REPOSITORY "https://gitlab.com/${ARGS_GITLAB_REPOSITORY}.git")
    elseif (DEFINED ARGS_BITBUCKET_REPOSITORY)
        set(ARGS_GIT_REPOSITORY "https://bitbucket.org/${ARGS_BITBUCKET_REPOSITORY}.git")
    elseif (DEFINED ARGS_GITEE_REPOSITORY)
        set(ARGS_GIT_REPOSITORY "https://gitee.com/${ARGS_GITEE_REPOSITORY}.git")
    endif ()

    if (DEFINED ARGS_GIT_REPOSITORY)
        list(APPEND ARGS_UNPARSED_ARGUMENTS GIT_REPOSITORY ${ARGS_GIT_REPOSITORY})
        if (NOT DEFINED ARGS_GIT_TAG)
            set(ARGS_GIT_TAG v${ARGS_VERSION})
        endif ()

        # 如果未指定包名，则尝试从 git 仓库地址推断
        if (NOT DEFINED ARGS_NAME)
            _get_package_name_from_git_uri(${ARGS_GIT_REPOSITORY} output_name)
            set(ARGS_NAME ${output_name})
        endif ()
    endif ()

    set(skip_fetch FALSE)

    if (DEFINED ARGS_GIT_TAG)
        list(APPEND ARGS_UNPARSED_ARGUMENTS GIT_TAG ${ARGS_GIT_TAG})
        # 如果显式指定了 GIT_SHALLOW，则使用该值
        if (DEFINED ARGS_GIT_SHALLOW)
            list(APPEND ARGS_UNPARSED_ARGUMENTS GIT_SHALLOW ${ARGS_GIT_SHALLOW})
        endif ()
        # 如果显式指定了 GIT_SUBMODULES_RECURSIVE，则使用该值
        if (DEFINED ARGS_GIT_SUBMODULES_RECURSIVE)
            list(APPEND ARGS_UNPARSED_ARGUMENTS GIT_SUBMODULES_RECURSIVE ${ARGS_GIT_SUBMODULES_RECURSIVE})
        endif ()
    endif ()

    if (DEFINED ARGS_URL)
        # 如果未指定包名或版本，则尝试从 URL 推断
        list(GET ARGS_URL 0 first_url)
        _get_package_name_and_ver_from_url(${first_url} name_from_url ver_from_url)
        # 如果第一个 URL 未能推断出包名和版本，可以尝试其他 URL，但实际很少见，这里不处理

        # 用户显式指定的包名和版本优先于推断结果
        if (NOT DEFINED ARGS_NAME)
            set(ARGS_NAME ${name_from_url})
        endif ()
        if (NOT DEFINED ARGS_VERSION)
            set(ARGS_VERSION ${ver_from_url})
        endif ()

        list(APPEND ARGS_UNPARSED_ARGUMENTS URL "${ARGS_URL}")
    endif ()

    # 检查必需参数

    if (NOT DEFINED ARGS_NAME)
        rendu_log_fatal("未提供 'NAME'，且无法自动推断，参数为: '${ARGN}'。")
    endif ()

    # ====================================================================
# 内部辅助函数 - 包状态检查（已在前面定义）
# ====================================================================

# ====================================================================
# 检查包是否已添加（代码块）
# ====================================================================
    _check_if_package_already_added(${ARGS_NAME} "${ARGS_VERSION}" package_already_added)
    if (package_already_added)
        _export_variables(${ARGS_NAME})
        rendu_log_debug("包 '${ARGS_NAME}' 已添加，跳过。")
        return()
    endif ()

    if (NOT ARGS_FORCE)
        if (RENDU_USE_LOCAL_PACKAGES OR RENDU_LOCAL_PACKAGES_ONLY)
            _find_package(find ${ARGS_NAME} "${ARGS_VERSION}" ${ARGS_FIND_PACKAGE_ARGUMENTS})

            if (find)
                _export_variables(${ARGS_NAME})
                return()
            endif ()

            if (RENDU_LOCAL_PACKAGES_ONLY)
                rendu_log_error("未通过 find_package(${ARGS_NAME} ${ARGS_VERSION}) 找到 ${ARGS_NAME}，且 RENDU_LOCAL_PACKAGES_ONLY 为 ON，无法继续。")
            endif ()
        endif ()
    endif ()

    rendu_register_package("${ARGS_NAME}" "${ARGS_VERSION}")

    if (DEFINED ARGS_GIT_TAG)
        set(package_info "${ARGS_GIT_TAG}")
    elseif (DEFINED ARGS_SOURCE_DIR)
        set(package_info "${ARGS_SOURCE_DIR}")
    else ()
        set(package_info "${ARGS_VERSION}")
    endif ()

    _add_patches(${ARGS_PATCHES})

    if (DEFINED ARGS_DOWNLOAD_COMMAND)
        list(APPEND ARGS_UNPARSED_ARGUMENTS DOWNLOAD_COMMAND ${ARGS_DOWNLOAD_COMMAND})
    elseif (DEFINED ARGS_SOURCE_DIR)
        list(APPEND ARGS_UNPARSED_ARGUMENTS SOURCE_DIR ${ARGS_SOURCE_DIR})
        if (NOT IS_ABSOLUTE ${ARGS_SOURCE_DIR})
            # 展开 ARGS_SOURCE_DIR 的相对路径。因为 EXISTS 不支持相对路径，这很重要。
            get_filename_component(
                    source_directory ${ARGS_SOURCE_DIR} REALPATH BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}
            )
        else ()
            set(source_directory ${ARGS_SOURCE_DIR})
        endif ()
        if (NOT EXISTS ${source_directory})
            string(TOLOWER ${ARGS_NAME} lower_case_name)
            # 移除时间戳，强制 CMake 重新下载依赖
            file(REMOVE_RECURSE "${RENDU_FETCHCONTENT_BASE_DIR}/${lower_case_name}-subbuild")
        endif ()
    elseif (NOT ARGS_NO_CACHE)
        string(TOLOWER ${ARGS_NAME} lower_case_name)
        # 只使用 NAME 和 GIT_TAG 计算哈希，避免 CMAKE_ARGS 变化导致重新下载源码
        set(origin_parameters "${ARGS_NAME};${ARGS_GIT_TAG}")
        if (ARGS_CUSTOM_CACHE_KEY)
            # 应用自定义唯一目录名
            set(download_directory ${RENDU_PACKAGES_CACHE}/${lower_case_name}/${ARGS_CUSTOM_CACHE_KEY})
        elseif (RENDU_USE_NAMED_CACHE_DIRECTORIES)
            string(SHA1 origin_hash "${origin_parameters};NEW_CACHE_STRUCTURE_TAG")
            _get_shortest_hash(
                    "${RENDU_PACKAGES_CACHE}/${lower_case_name}" # 源码缓存目录
                    "${origin_hash}" # 输入哈希
                    short_hash # 计算出的哈希
            )
            set(download_directory ${RENDU_PACKAGES_CACHE}/${lower_case_name}/${short_hash}/${ARGS_NAME})
        else ()
            string(SHA1 origin_hash "${origin_parameters}")
            _get_shortest_hash(
                    "${RENDU_PACKAGES_CACHE}/${lower_case_name}" # 源码缓存目录
                    "${origin_hash}" # 输入哈希
                    short_hash # 计算出的哈希
            )
            set(download_directory ${RENDU_PACKAGES_CACHE}/${lower_case_name}/${short_hash})
        endif ()
        # 展开 download_directory 的相对路径。因为 EXISTS 不支持相对路径，这很重要。
        get_filename_component(download_directory ${download_directory} ABSOLUTE)
        list(APPEND ARGS_UNPARSED_ARGUMENTS SOURCE_DIR ${download_directory})

        file(LOCK ${download_directory}/../cmake.lock)

        if (EXISTS ${download_directory})
            file(LOCK ${download_directory}/../cmake.lock RELEASE)

            rendu_log_info("使用缓存包: ${ARGS_NAME} (来自 ${download_directory})")

            _store_fetch_properties(
                    ${ARGS_NAME} "${download_directory}"
                    "${RENDU_FETCHCONTENT_BASE_DIR}/${lower_case_name}-build"
            )
            _get_fetch_properties("${ARGS_NAME}" pkg_source_dir pkg_binary_dir)

            if (DEFINED ARGS_GIT_TAG AND NOT (PATCH_COMMAND IN_LIST ARGS_UNPARSED_ARGUMENTS))
                # 如果缓存自检失败，发出警告
                _check_git_working_dir_is_clean(${download_directory} ${ARGS_GIT_TAG} is_clean)
                if (NOT ${is_clean})
                    rendu_log_warn("缓存 ${ARGS_NAME} (${download_directory}) 的工作目录未干净，可能需要手动清理。")
                endif ()
            endif ()

            _add_subdirectory(
                    "${ARGS_NAME}"
                    "${download_only}"
                    "${pkg_source_dir}/${ARGS_SOURCE_SUBDIR}"
                    "${pkg_binary_dir}"
                    "${ARGS_EXCLUDE_FROM_ALL}"
                    "${ARGS_SYSTEM}"
                    "${ARGS_OPTIONS}"
            )
            set(package_info "${package_info} at ${download_directory}")

            # 源码目录已缓存/填充，重载 FetchContent 调用
            set(skip_fetch TRUE)
            _override_fetchcontent(
                    "${lower_case_name}" SOURCE_DIR "${RENDU_PACKAGE_${ARGS_NAME}_SOURCE_DIR}/${ARGS_SOURCE_SUBDIR}"
                    BINARY_DIR "${RENDU_PACKAGE_${ARGS_NAME}_BINARY_DIR}"
            )

        else ()
            # 如果 GIT_TAG 不是提交哈希，则启用浅克隆。虽然不一定准确，但能保证不会误判哈希。
            if (NOT DEFINED ARGS_GIT_SHALLOW)
                _is_git_tag_commit_hash("${ARGS_GIT_TAG}" is_hash)
                if (NOT ${is_hash})
                    list(APPEND ARGS_UNPARSED_ARGUMENTS GIT_SHALLOW TRUE)
                endif ()
            endif ()

            # 移除时间戳，强制 CMake 重新下载依赖
            file(REMOVE_RECURSE ${RENDU_FETCHCONTENT_BASE_DIR}/${lower_case_name}-subbuild)
            set(package_info "${package_info} to ${download_directory}")
        endif ()
    endif ()

    if (NOT "${download_only}")
        _create_module_file(${ARGS_NAME} "rendu_add_package(\"${ARGN}\")")
    endif ()

    if (RENDU_PACKAGE_LOCK_ENABLED)
        if ((ARGS_VERSION AND NOT ARGS_SOURCE_DIR) OR RENDU_INCLUDE_ALL_IN_PACKAGE_LOCK)
            _add_to_package_lock(${ARGS_NAME} "${ARGN}")
        elseif (ARGS_SOURCE_DIR)
            _add_comment_to_package_lock(${ARGS_NAME} "local directory")
        else ()
            _add_comment_to_package_lock(${ARGS_NAME} "${ARGN}")
        endif ()
    endif ()

    rendu_log_info(" 添加包 ${ARGS_NAME}@${ARGS_VERSION} (${package_info})")

    if (NOT skip_fetch)
        # CMake 3.28 增加了 EXCLUDE、SYSTEM（3.25）和 SOURCE_SUBDIR（3.18）参数到 FetchContent_Declare。
        # 调用 FetchContent_MakeAvailable 会自动转发这些参数到 add_subdirectory。
        # 在此之前需手动调用 FetchContent_Populate 和 add_subdirectory，3.30 起已废弃。
        # CMake 的一个 bug 导致 3.30.3 之前无法用非废弃方式。
        set(fetchContentDeclareExtraArgs "")
        if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.30.3")
            if (${ARGS_EXCLUDE_FROM_ALL})
                list(APPEND fetchContentDeclareExtraArgs EXCLUDE_FROM_ALL)
            endif ()
            if (${ARGS_SYSTEM})
                list(APPEND fetchContentDeclareExtraArgs SYSTEM)
            endif ()
            if (DEFINED ARGS_SOURCE_SUBDIR)
                list(APPEND fetchContentDeclareExtraArgs SOURCE_SUBDIR ${ARGS_SOURCE_SUBDIR})
            endif ()
            # 将 OPTIONS 转换为 CMAKE_ARGS 格式并添加到未解析参数中
            if (ARGS_OPTIONS)
                foreach (OPTION ${ARGS_OPTIONS})
                    _parse_option("${OPTION}" opt_key opt_value)
                    list(APPEND ARGS_UNPARSED_ARGUMENTS CMAKE_ARGS "-D${opt_key}=${opt_value}")
                endforeach ()
            endif ()
        endif ()
        rendu_fetchcontent_declare("${ARGS_NAME}" ${fetchContentDeclareExtraArgs} "${ARGS_UNPARSED_ARGUMENTS}")

        rendu_fetch_content("${ARGS_NAME}" ${download_only} populated ${ARGS_UNPARSED_ARGUMENTS})
        if (download_directory)
            file(LOCK ${download_directory}/../cmake.lock RELEASE)
        endif ()
        if (${populated} AND ${CMAKE_VERSION} VERSION_LESS "3.30.3")
            _add_subdirectory(
                    "${ARGS_NAME}"
                    "${download_only}"
                    "${${ARGS_NAME}_SOURCE_DIR}/${ARGS_SOURCE_SUBDIR}"
                    "${${ARGS_NAME}_BINARY_DIR}"
                    "${ARGS_EXCLUDE_FROM_ALL}"
                    "${ARGS_SYSTEM}"
                    "${ARGS_OPTIONS}"
            )
        endif ()
        _get_fetch_properties(${ARGS_NAME} sourceDir binaryDir)
    endif ()

    set(${ARGS_NAME}_ADDED YES)
    _export_variables("${ARGS_NAME}")
endfunction()

# ====================================================================
# 获取已声明包
# 描述: 获取已声明包
#
# 参数:
#   pkgName - 包名
# ====================================================================
macro(rendu_get_package pkgName)
    if (DEFINED "RENDU_DECLARATION_${pkgName}")
        rendu_add_package(NAME ${pkgName})
    else ()
        rendu_log_error("无法获取包 ${pkgName}：没有可用的声明")
    endif ()
endmacro()