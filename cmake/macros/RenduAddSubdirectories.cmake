# ====================================================================
# 模块: RenduAddSubdirectories
# 描述: 增强版 add_subdirectory，自动添加所有子目录为子项目
# 依赖模块:
#   - RenduLogging (日志)
# ====================================================================

# ====================================================================
# 函数: rendu_add_subdirectories
# 描述: 自动添加指定目录下所有子文件夹为子项目，支持排除指定目录
#
# 参数:
#   DIR           - 根目录 (必填)
#   EXCLUDE_DIRS  - 需要排除的子目录列表 (可选，绝对或相对路径均可)
#   RECURSIVE     - 是否递归添加子目录 (可选)
#
# 用法示例:
#   rendu_add_subdirectories(
#       DIR ${CMAKE_CURRENT_SOURCE_DIR}
#       EXCLUDE_DIRS
#           ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
#           ${CMAKE_CURRENT_SOURCE_DIR}/tests
#   )
# ====================================================================
function(rendu_add_subdirectories)
    # 参数解析
    set(options RECURSIVE)
    set(oneValueArgs DIR)
    set(multiValueArgs EXCLUDE_DIRS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 检查必需参数
    if (NOT ARG_DIR)
        rendu_log_fatal("rendu_add_subdirectories: 必须指定 DIR")
    endif ()
    if (NOT IS_DIRECTORY "${ARG_DIR}")
        rendu_log_warn("rendu_add_subdirectories: 目录不存在: ${ARG_DIR}")
        return()
    endif ()

# ====================================================================
# 优化：规范化排除目录为绝对路径并去除末尾斜杠（避免重复计算）
# 性能改进: 预处理排除目录，减少运行时计算开销
# ====================================================================
    set(EXCLUDE_ABS_DIRS "")
    if (ARG_EXCLUDE_DIRS)
        foreach (excl IN LISTS ARG_EXCLUDE_DIRS)
            if (IS_ABSOLUTE "${excl}")
                file(TO_CMAKE_PATH "${excl}" excl_norm)
            else ()
                get_filename_component(excl_norm "${ARG_DIR}/${excl}" ABSOLUTE)
            endif ()
            string(REGEX REPLACE "/$" "" excl_norm "${excl_norm}")
            list(APPEND EXCLUDE_ABS_DIRS "${excl_norm}")
        endforeach ()
    endif ()

    # 查找所有一级子目录
    file(GLOB children RELATIVE "${ARG_DIR}" "${ARG_DIR}/*")
    foreach (child IN LISTS children)
        set(child_path "${ARG_DIR}/${child}")
        if (IS_DIRECTORY "${child_path}")
            get_filename_component(child_abs "${child_path}" ABSOLUTE)
            string(REGEX REPLACE "/$" "" child_abs "${child_abs}")

# ====================================================================
# 优化：只有存在排除列表时才进行查找
# 性能改进: 条件性查找，减少不必要的列表操作
# ====================================================================
            set(found -1)
            if (EXCLUDE_ABS_DIRS)
                list(FIND EXCLUDE_ABS_DIRS "${child_abs}" found)
            endif ()

            if (found EQUAL -1)
                add_subdirectory("${child_path}")
                rendu_log_info("添加子目录: ${child_path}")

# ====================================================================
# 递归添加子目录（当 RECURSIVE 选项启用时）
# 说明: 递归调用当前函数处理子目录，保持相同的排除规则
# ====================================================================
                if (ARG_RECURSIVE)
                    rendu_add_subdirectories(DIR "${child_path}" EXCLUDE_DIRS ${ARG_EXCLUDE_DIRS} RECURSIVE)
                endif ()
            endif ()
        endif ()
    endforeach ()
endfunction()
