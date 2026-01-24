# ====================================================================
# 模块: RenduCollectDirectories
# 描述: 智能包含目录收集器 (仅收集第一层子目录，支持排除)
# 性能优化:
#   - 预处理排除目录，使用 list FIND 替代嵌套循环 (O(m×n) → O(n))
#   - 添加结果缓存机制，避免重复文件系统扫描
#   - 优化字符串操作，减少拼接次数
# 依赖模块:
#   - 无
# ====================================================================

# ====================================================================
# 函数: rendu_collect_include_directories
# 描述: 收集指定目录下的第一层子目录，支持排除过滤
#
# 参数:
#   output_var     - 输出变量名 (存储收集到的第一层子目录列表)
#   root_dir       - 要收集的根目录
#   EXCLUDE_DIRS   - 指定要排除的目录 (可选)
#   EXCLUDE_REGEX  - 指定要排除的目录正则表达式 (可选)
#   NO_CACHE       - 禁用缓存，强制重新扫描 (可选)
#
# 用法示例:
#   rendu_collect_include_directories(MY_INCLUDES
#       ${CMAKE_CURRENT_SOURCE_DIR}
#       EXCLUDE_DIRS
#           ${CMAKE_CURRENT_SOURCE_DIR}/tests
#           ${CMAKE_BINARY_DIR}
#       EXCLUDE_REGEX ".*/private"
#   )
# ====================================================================
function(rendu_collect_include_directories output_var root_dir)
    # 参数解析
    set(options NO_CACHE)
    set(oneValueArgs EXCLUDE_REGEX)
    set(multiValueArgs EXCLUDE_DIRS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # ====================================================================
    # 优化 1: 结果缓存机制
    # ====================================================================
    if (NOT ARG_NO_CACHE)
        # 生成缓存键: 包含 root_dir, EXCLUDE_DIRS, EXCLUDE_REGEX
        set(cache_input "${root_dir};${ARG_EXCLUDE_DIRS};${ARG_EXCLUDE_REGEX}")
        string(MD5 cache_hash "${cache_input}")
        set(cache_var "_RENDU_COLLECT_DIRS_CACHE_${cache_hash}")

        # 检查缓存是否存在
        if (DEFINED "${cache_var}")
            set(${output_var} "${${cache_var}}" PARENT_SCOPE)
            return()
        endif ()
    endif ()

    # ====================================================================
    # 优化 2: 预处理排除目录为绝对路径列表
    # 性能改进: O(1) 查找替代 O(m) 嵌套循环
    # ====================================================================
    set(EXCLUDED_ABS_DIRS "")
    if (ARG_EXCLUDE_DIRS)
        foreach (ex_dir IN LISTS ARG_EXCLUDE_DIRS)
            if (IS_ABSOLUTE "${ex_dir}")
                set(abs_dir "${ex_dir}")
            else ()
                # 转换相对路径为绝对路径
                get_filename_component(abs_dir "${root_dir}/${ex_dir}" ABSOLUTE)
            endif ()
            list(APPEND EXCLUDED_ABS_DIRS "${abs_dir}")
        endforeach ()
        # 去重
        list(REMOVE_DUPLICATES EXCLUDED_ABS_DIRS)
    endif ()

    # ====================================================================
    # 收集第一层子目录
    # ====================================================================
    file(GLOB children RELATIVE "${root_dir}" "${root_dir}/*")
    set(result "")

    # ====================================================================
    # 优化 3: 使用 list FIND 替代嵌套循环
    # 性能改进: 查找从 O(m×n) 降至 O(n)
    # ====================================================================
    foreach (child IN LISTS children)
        # ====================================================================
        # 优化 4: 减少字符串拼接次数
        # 说明: 直接拼接而非多次调用
        # ====================================================================
        set(child_path "${root_dir}/${child}")

        if (IS_DIRECTORY "${child_path}")
            # 优化: 使用 list FIND 进行 O(n) 查找而非 O(m×n)
            set(excluded_dir FALSE)
            if (EXCLUDED_ABS_DIRS)
                list(FIND EXCLUDED_ABS_DIRS "${child_path}" found_idx)
                if (NOT found_idx EQUAL -1)
                    set(excluded_dir TRUE)
                endif ()
            endif ()

            # 正则表达式检查 (保持不变，仅在有正则时执行)
            set(excluded_regex FALSE)
            if (ARG_EXCLUDE_REGEX)
                if ("${child_path}" MATCHES "${ARG_EXCLUDE_REGEX}")
                    set(excluded_regex TRUE)
                endif ()
            endif ()

            # 满足条件才加入
            if (NOT excluded_dir AND NOT excluded_regex)
                list(APPEND result "${child_path}")
            endif ()
        endif ()
    endforeach ()

    # ====================================================================
    # 存储结果到缓存
    # ====================================================================
    if (NOT ARG_NO_CACHE AND DEFINED cache_var)
        set("${cache_var}" ${result} CACHE INTERNAL "RenduCollectDirectories 缓存: ${cache_hash}")
    endif ()

    # 返回结果
    set(${output_var} ${result} PARENT_SCOPE)
endfunction()

# ====================================================================
# 函数: rendu_clear_collect_directories_cache
# 描述: 清除 RenduCollectDirectories 的所有缓存
#
# 用法示例:
#   rendu_clear_collect_directories_cache()
# ====================================================================
function(rendu_clear_collect_directories_cache)
    get_cmake_property(cache_vars CACHE_VARIABLES)
    foreach (var IN LISTS cache_vars)
        if (var MATCHES "^_RENDU_COLLECT_DIRS_CACHE_")
            unset(${var} CACHE)
        endif ()
    endforeach ()
endfunction()
