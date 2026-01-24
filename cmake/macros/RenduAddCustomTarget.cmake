# ====================================================================
# 模块: RenduAddCustomTarget
# 描述: 增强版自定义目标和命令创建
# 依赖模块:
#   - RenduLogging (日志)
#   - RenduCollectDirectories (目录收集)
#   - RenduCollectFiles (文件收集)
#   - RenduSourceGroup (源文件分组)
#
# 建议通过 RenduCore.cmake 统一加载所有模块
# ====================================================================

# ====================================================================
# 函数: rendu_add_custom_target
# 描述: 增强版 add_custom_target，支持源文件收集、依赖管理、IDE 集成
#
# 参数:
#   DIR                - 源码目录 (可选，用于自动收集)
#   PROJECT            - 项目名 (可选，用于 IDE 工程标签)
#   NAME               - 目标名 (必填)
#   COMMAND            - 命令 (必填，可以是命令列表)
#   COMMAND_ARGS       - 命令参数列表 (可选)
#   WORKING_DIR        - 工作目录 (可选，默认为构建目录)
#   OUTPUT             - 输出文件列表 (可选，用于 add_custom_command)
#   DEPENDS            - 依赖目标列表 (可选)
#   BYPRODUCTS         - 副产物文件列表 (可选)
#   SOURCES            - 指定源文件列表 (可选，未指定则自动收集)
#   DEFINES            - 需要添加的预处理宏 (可选)
#   COMMENT            - 命令注释 (可选)
#   VERBATIM           - 是否使用 VERBATIM 模式 (可选，默认 ON)
#   COMMAND_EXPAND_LISTS - 是否展开列表参数 (可选，默认 OFF)
#   JOB_POOL           - 作业池名称 (可选)
#   ALL                - 是否将目标添加到 ALL 目标 (可选，默认 OFF)
#
# 用法示例:
#   # 简单自定义目标
#   rendu_add_custom_target(
#       NAME        generate_code
#       COMMAND     python3
#       COMMAND_ARGS generate.py --output generated/
#       WORKING_DIR ${CMAKE_CURRENT_SOURCE_DIR}
#       COMMENT     "Generating code..."
#   )
#
#   # 带源文件收集的自定义目标
#   rendu_add_custom_target(
#       DIR         ${CMAKE_CURRENT_SOURCE_DIR}
#       PROJECT     ${PROJECT_NAME}
#       NAME        compile_shaders
#       COMMAND     glslc
#       COMMAND_ARGS ${shader_files} -o ${output_files}
#       DEPENDS     rendu::core
#       ALL         ON
#       COMMENT     "Compiling shaders"
#   )
#
#   # 带输出的自定义命令目标
#   rendu_add_custom_target(
#       DIR         ${CMAKE_CURRENT_SOURCE_DIR}
#       NAME        generate_header
#       COMMAND     python3
#       COMMAND_ARGS tools/gen_header.py
#       OUTPUT      ${CMAKE_CURRENT_BINARY_DIR}/generated.hpp
#       DEPENDS     ${source_files}
#       COMMENT     "Generating header file"
#   )
# ====================================================================
function(rendu_add_custom_target)
    # 参数解析
    set(options ALL VERBATIM COMMAND_EXPAND_LISTS)
    set(oneValueArgs DIR PROJECT NAME WORKING_DIR COMMENT JOB_POOL)
    set(multiValueArgs COMMAND COMMAND_ARGS OUTPUT DEPENDS BYPRODUCTS SOURCES DEFINES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 检查必需参数
    if (NOT ARG_NAME)
        rendu_log_fatal("rendu_add_custom_target: 必须指定 NAME")
    endif ()
    if (NOT ARG_COMMAND)
        rendu_log_fatal("rendu_add_custom_target: 必须指定 COMMAND")
    endif ()

    # 检查命令是否有效
    if ("${ARG_COMMAND}" STREQUAL "")
        rendu_log_fatal("rendu_add_custom_target: COMMAND 参数为空字符串")
    endif ()

    # 设置默认值
    if (NOT ARG_WORKING_DIR)
        set(ARG_WORKING_DIR "${CMAKE_BINARY_DIR}")
    endif ()

    if (NOT DEFINED ARG_VERBATIM)
        set(ARG_VERBATIM ON)
    endif ()

    # 生成目标名称
    set(target_name "${ARG_NAME}")
    if (ARG_PROJECT)
        set(target_name "${ARG_PROJECT}_${ARG_NAME}")
    endif ()

    # 收集源文件
    set(source_files "")
    if (ARG_DIR AND NOT ARG_SOURCES)
        if (NOT IS_DIRECTORY "${ARG_DIR}")
            rendu_log_warn("rendu_add_custom_target: 源码目录不存在: ${ARG_DIR}")
        else ()
            rendu_collect_source_files(src_list "${ARG_DIR}")
            set(source_files ${src_list})
            rendu_log_debug("自动收集到 ${src_list} 个源文件")
        endif ()
    elseif (ARG_SOURCES)
        set(source_files ${ARG_SOURCES})
    endif ()

    # 构建命令列表
    set(full_command ${ARG_COMMAND})
    if (ARG_COMMAND_ARGS)
        list(APPEND full_command ${ARG_COMMAND_ARGS})
    endif ()

    # 日志：调试命令构建
    rendu_log_debug("命令: ${full_command}")
    rendu_log_debug("命令元素数量: [${full_command}]")

    # 根据是否有输出决定使用 add_custom_target 还是 add_custom_command + add_custom_target
    if (ARG_OUTPUT)
        # 使用 add_custom_command 创建输出规则
        if (NOT ARG_DIR)
            set(ARG_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        endif ()

        add_custom_command(
            OUTPUT ${ARG_OUTPUT}
            COMMAND ${full_command}
            WORKING_DIRECTORY ${ARG_WORKING_DIR}
            DEPENDS ${ARG_DEPENDS} ${source_files}
            BYPRODUCTS ${ARG_BYPRODUCTS}
            COMMENT "${ARG_COMMENT}"
            VERBATIM ${ARG_VERBATIM}
            COMMAND_EXPAND_LISTS ${ARG_COMMAND_EXPAND_LISTS}
        )

        # 创建自定义目标依赖于输出文件
        if (ARG_ALL)
            add_custom_target(${target_name} ALL
                DEPENDS ${ARG_OUTPUT}
                SOURCES ${source_files}
            )
        else ()
            add_custom_target(${target_name}
                DEPENDS ${ARG_OUTPUT}
                SOURCES ${source_files}
            )
        endif ()

        rendu_log_info("添加自定义命令目标 ${target_name}")
    else ()
        # 使用 add_custom_target
        # 由于 full_command 可能是列表，我们需要显式展开
        # 第一个元素是命令本身，其余是参数
        list(LENGTH full_command cmd_length)
        if (cmd_length GREATER 0)
            list(GET full_command 0 cmd_executable)
        else ()
            set(cmd_executable "")
        endif ()

        # 提取命令参数（跳过第一个元素，因为它是可执行文件）
        if (cmd_length GREATER 1)
            list(SUBLIST full_command 1 -1 cmd_rest)
        else ()
            set(cmd_rest "")
        endif ()

        # 构建 add_custom_target 调用
        if (ARG_ALL)
            if (cmd_rest)
                add_custom_target(${target_name} ALL
                    COMMAND ${cmd_executable} ${cmd_rest}
                    WORKING_DIRECTORY ${ARG_WORKING_DIR}
                    DEPENDS ${ARG_DEPENDS}
                    BYPRODUCTS ${ARG_BYPRODUCTS}
                    SOURCES ${source_files}
                    COMMENT "${ARG_COMMENT}"
                    VERBATIM ${ARG_VERBATIM}
                    JOB_POOL ${ARG_JOB_POOL}
                )
            else ()
                add_custom_target(${target_name} ALL
                    COMMAND ${cmd_executable}
                    WORKING_DIRECTORY ${ARG_WORKING_DIR}
                    DEPENDS ${ARG_DEPENDS}
                    BYPRODUCTS ${ARG_BYPRODUCTS}
                    SOURCES ${source_files}
                    COMMENT "${ARG_COMMENT}"
                    VERBATIM ${ARG_VERBATIM}
                    JOB_POOL ${ARG_JOB_POOL}
                )
            endif ()
        else ()
            if (cmd_rest)
                add_custom_target(${target_name}
                    COMMAND ${cmd_executable} ${cmd_rest}
                    WORKING_DIRECTORY ${ARG_WORKING_DIR}
                    DEPENDS ${ARG_DEPENDS}
                    BYPRODUCTS ${ARG_BYPRODUCTS}
                    SOURCES ${source_files}
                    COMMENT "${ARG_COMMENT}"
                    VERBATIM ${ARG_VERBATIM}
                    JOB_POOL ${ARG_JOB_POOL}
                )
            else ()
                add_custom_target(${target_name}
                    COMMAND ${cmd_executable}
                    WORKING_DIRECTORY ${ARG_WORKING_DIR}
                    DEPENDS ${ARG_DEPENDS}
                    BYPRODUCTS ${ARG_BYPRODUCTS}
                    SOURCES ${source_files}
                    COMMENT "${ARG_COMMENT}"
                    VERBATIM ${ARG_VERBATIM}
                    JOB_POOL ${ARG_JOB_POOL}
                )
            endif ()
        endif ()

        rendu_log_info("添加自定义目标 ${target_name}: ${cmd_executable}")
        if (cmd_rest)
            rendu_log_debug("  参数: ${cmd_rest}")
        endif ()
    endif ()

    # 设置 IDE 项目标签
    if (ARG_PROJECT)
        set_target_properties(${target_name} PROPERTIES PROJECT_LABEL "${ARG_PROJECT}")
        set_target_properties(${target_name} PROPERTIES FOLDER "${ARG_PROJECT}/CustomTargets")
    endif ()

    # 设置源文件组 (如果有源文件)
    if (source_files)
        if (NOT ARG_DIR)
            set(ARG_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        endif ()
        rendu_source_group("${ARG_DIR}" ${source_files})
    endif ()

    # 添加预处理宏 (用于源文件组显示)
    if (ARG_DEFINES AND source_files)
        rendu_log_debug("定义宏 ${ARG_DEFINES}")
    endif ()

    # 日志输出
    if (ARG_COMMENT)
        rendu_log_info("  注释: ${ARG_COMMENT}")
    endif ()
    if (ARG_DEPENDS)
        rendu_log_debug("  依赖: ${ARG_DEPENDS}")
    endif ()
    if (ARG_OUTPUT)
        rendu_log_debug("  输出: ${ARG_OUTPUT}")
    endif ()
endfunction()

# ====================================================================
# 函数: rendu_add_custom_command
# 描述: 增强版 add_custom_command，支持自动源文件收集
#
# 参数:
#   DIR                - 源码目录 (可选，用于自动收集)
#   PROJECT            - 项目名 (可选，用于 IDE 工程标签)
#   OUTPUT             - 输出文件 (必填)
#   COMMAND            - 命令 (必填)
#   COMMAND_ARGS       - 命令参数列表 (可选)
#   WORKING_DIR        - 工作目录 (可选)
#   DEPENDS            - 依赖目标或文件列表 (可选)
#   BYPRODUCTS         - 副产物文件列表 (可选)
#   SOURCES            - 指定源文件列表 (可选，未指定则自动收集)
#   COMMENT            - 命令注释 (可选)
#   VERBATIM           - 是否使用 VERBATIM 模式 (可选，默认 ON)
#   COMMAND_EXPAND_LISTS - 是否展开列表参数 (可选，默认 OFF)
#   DEPENDS_EXPLICIT   - 是否显式依赖 (可选，默认 OFF)
#
# 用法示例:
#   rendu_add_custom_command(
#       DIR         ${CMAKE_CURRENT_SOURCE_DIR}
#       PROJECT     ${PROJECT_NAME}
#       OUTPUT      ${CMAKE_CURRENT_BINARY_DIR}/generated.cpp
#       COMMAND     python3
#       COMMAND_ARGS tools/generate.py
#       DEPENDS     rendu::core
#       COMMENT     "Generating generated.cpp"
#   )
# ====================================================================
function(rendu_add_custom_command)
    # 参数解析
    set(options VERBATIM COMMAND_EXPAND_LISTS DEPENDS_EXPLICIT)
    set(oneValueArgs DIR PROJECT OUTPUT WORKING_DIR COMMENT)
    set(multiValueArgs COMMAND COMMAND_ARGS DEPENDS BYPRODUCTS SOURCES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 检查必需参数
    if (NOT ARG_OUTPUT)
        rendu_log_fatal("rendu_add_custom_command: 必须指定 OUTPUT")
    endif ()
    if (NOT ARG_COMMAND)
        rendu_log_fatal("rendu_add_custom_command: 必须指定 COMMAND")
    endif ()
    if ("${ARG_COMMAND}" STREQUAL "")
        rendu_log_fatal("rendu_add_custom_command: COMMAND 参数为空字符串")
    endif ()

    # 设置默认值
    if (NOT ARG_WORKING_DIR)
        set(ARG_WORKING_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    endif ()

    if (NOT DEFINED ARG_VERBATIM)
        set(ARG_VERBATIM ON)
    endif ()

    # 收集源文件
    set(source_files "")
    if (ARG_DIR AND NOT ARG_SOURCES)
        if (NOT IS_DIRECTORY "${ARG_DIR}")
            rendu_log_warn("rendu_add_custom_command: 源码目录不存在: ${ARG_DIR}")
        else ()
            rendu_collect_source_files(src_list "${ARG_DIR}")
            set(source_files ${src_list})
        endif ()
    elseif (ARG_SOURCES)
        set(source_files ${ARG_SOURCES})
    endif ()

    # 构建命令列表
    set(full_command ${ARG_COMMAND})
    if (ARG_COMMAND_ARGS)
        list(APPEND full_command ${ARG_COMMAND_ARGS})
    endif ()

    # 日志：调试命令构建
    rendu_log_debug("命令: ${full_command}")
    rendu_log_debug("命令元素数量: [${full_command}]")

    # 创建自定义命令
    add_custom_command(
        OUTPUT ${ARG_OUTPUT}
        COMMAND ${full_command}
        WORKING_DIRECTORY ${ARG_WORKING_DIR}
        DEPENDS ${ARG_DEPENDS} ${source_files}
        BYPRODUCTS ${ARG_BYPRODUCTS}
        COMMENT "${ARG_COMMENT}"
        VERBATIM ${ARG_VERBATIM}
        COMMAND_EXPAND_LISTS ${ARG_COMMAND_EXPAND_LISTS}
        DEPENDS_EXPLICIT ${ARG_DEPENDS_EXPLICIT}
        SOURCES ${source_files}
    )

    rendu_log_info("添加自定义命令")
    rendu_log_info("  输出: ${ARG_OUTPUT}")
    if (ARG_COMMENT)
        rendu_log_info("  注释: ${ARG_COMMENT}")
    endif ()
    if (ARG_DEPENDS)
        rendu_log_debug("  依赖: ${ARG_DEPENDS}")
    endif ()
endfunction()
