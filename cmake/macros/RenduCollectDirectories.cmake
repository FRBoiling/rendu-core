# ====================================================================
# 模块: RenduCollectDirectories
# 描述: 智能包含目录收集器 (仅收集第一层子目录，支持排除)
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
    set(options "")
    set(oneValueArgs EXCLUDE_REGEX)
    set(multiValueArgs EXCLUDE_DIRS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 收集第一层子目录
    file(GLOB children RELATIVE "${root_dir}" "${root_dir}/*")
    set(result "")

    foreach (child IN LISTS children)
        set(child_path "${root_dir}/${child}")
        if (IS_DIRECTORY "${child_path}")
            # 优化：只有存在排除列表时才检查
            set(excluded_dir FALSE)
            if (ARG_EXCLUDE_DIRS)
                foreach (ex_dir IN LISTS ARG_EXCLUDE_DIRS)
                    if ("${child_path}" STREQUAL "${ex_dir}")
                        set(excluded_dir TRUE)
                        break()
                    endif ()
                endforeach ()
            endif ()

            # 优化：只有存在正则表达式时才检查
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

    # 返回结果
    set(${output_var} ${result} PARENT_SCOPE)
endfunction()
