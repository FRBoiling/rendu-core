# ====================================================================
# 模块: RenduAddExecutable
# 描述: 增强版可执行目标创建
# 依赖模块:
#   - RenduLogging (日志)
#   - RenduCollectDirectories (目录收集)
#   - RenduCollectFiles (文件收集)
#   - RenduSourceGroup (源文件分组)
#
# 建议通过 RenduCore.cmake 统一加载所有模块
# ====================================================================

# ====================================================================
# 函数: rendu_add_executable
# 描述: 增强版 add_executable，自动收集源文件并标准化目标属性
#
# 参数:
#   DIR          - 源码目录 (必填)
#   PROJECT      - 项目名 (可选，用于 IDE 工程标签)
#   NAME         - 目标名 (必填)
#   PRIVATE_LINK - 需要 PRIVATE 链接的接口库 (可选)
#   PUBLIC_LINK  - 需要 PUBLIC 链接的依赖库 (可选)
#   DEFINES      - 需要添加的预处理宏 (可选)
#   SOURCES      - 指定源文件列表 (可选，未指定则自动收集)
#   ALIAS        - 是否创建 ALIAS 目标 (可选，ON 时自动命名为 project::name)
#
# 用法示例:
#   rendu_add_executable(
#       DIR          ${CMAKE_CURRENT_SOURCE_DIR}
#       PROJECT      ${PROJECT_NAME}
#       NAME         myexe
#       PRIVATE_LINK rendu-core-interface
#       PUBLIC_LINK  rendu::core
#       DEFINES      MYEXE_EXPORTS
#       ALIAS        ON
#   )
# ====================================================================
function(rendu_add_executable)
    # 参数解析
    set(options ALIAS)
    set(oneValueArgs DIR PROJECT NAME)
    set(multiValueArgs PRIVATE_LINK PUBLIC_LINK DEFINES SOURCES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 检查必需参数
    if (NOT ARG_NAME)
        rendu_log_fatal("rendu_add_executable: 必须指定 NAME")
    endif ()
    if (NOT ARG_DIR)
        rendu_log_fatal("rendu_add_executable: 必须指定 DIR")
    endif ()
    if (NOT IS_DIRECTORY "${ARG_DIR}")
        rendu_log_fatal("rendu_add_executable: 源码目录不存在: ${ARG_DIR}")
    endif ()

    # 收集源文件
    if (NOT ARG_SOURCES)
        rendu_collect_source_files(src_list "${ARG_DIR}")
    else ()
        set(src_list ${ARG_SOURCES})
    endif ()

    # 检查是否有源文件
    if (NOT src_list)
        rendu_log_warn("rendu_add_executable: 未找到源文件")
    endif ()

    # 生成目标名称
    set(target_name "${ARG_PROJECT}_${ARG_NAME}")
    add_executable(${target_name} ${src_list})

    # 自动收集 include 目录
    rendu_collect_include_directories(include_dirs "${ARG_DIR}"
            EXCLUDE_DIRS
            "${ARG_DIR}/tests"
            "${CMAKE_BINARY_DIR}"
            EXCLUDE_REGEX ".*/private"
    )

    target_include_directories(${target_name} PUBLIC ${include_dirs})

    # 设置目标属性
    if (ARG_PROJECT)
        set_target_properties(${target_name} PROPERTIES PROJECT_LABEL "${ARG_PROJECT}")
        set_target_properties(${target_name} PROPERTIES FOLDER "${ARG_PROJECT}/${ARG_NAME}")
    endif ()

    # 链接接口库
    if (ARG_PRIVATE_LINK)
        target_link_libraries(${target_name} PRIVATE ${ARG_PRIVATE_LINK})
    endif ()

    # 链接依赖
    if (ARG_PUBLIC_LINK)
        target_link_libraries(${target_name} PUBLIC ${ARG_PUBLIC_LINK})
    endif ()

    # 添加预处理宏
    if (ARG_DEFINES)
        target_compile_definitions(${target_name} PRIVATE ${ARG_DEFINES})
    endif ()

    # 创建 ALIAS 目标，命名规范为 project::name
    if (ARG_ALIAS AND ARG_PROJECT)
        set(alias_name "${ARG_PROJECT}::${ARG_NAME}")
        add_executable(${alias_name} ALIAS ${target_name})
        rendu_log_debug("${alias_name} 作为 ALIAS 目标")
    endif ()

    rendu_log_info("添加可执行目标 ${target_name}")
endfunction()
