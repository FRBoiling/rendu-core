# ====================================================================
# 模块: RenduAddLibrary
# 描述: 增强版库目标创建
# 依赖模块:
#   - RenduLogging (日志)
#   - RenduCollectDirectories (目录收集)
#   - RenduCollectFiles (文件收集)
#   - RenduSourceGroup (源文件分组)
#
# 建议通过 RenduCore.cmake 统一加载所有模块
# ====================================================================

# ====================================================================
# 函数: rendu_add_library
# 描述: 增强版 add_library，自动收集源文件并标准化目标属性
#
# 参数:
#   DIR          - 源码目录 (必填)
#   PROJECT      - 项目名 (可选，用于 IDE 工程标签)
#   NAME         - 目标名 (必填)
#   PRIVATE_LINK - 需要 PRIVATE 链接的接口库 (可选)
#   PUBLIC_LINK  - 需要 PUBLIC 链接的依赖库 (可选)
#   DEFINES      - 需要添加的预处理宏 (可选)
#   SOURCES      - 指定源文件列表 (可选，未指定则自动收集)
#   STATIC       - 创建静态库 (可选)
#   SHARED       - 创建动态库 (可选)
#   INTERFACE    - 创建接口库 (可选)
#   ALIAS        - 是否创建 ALIAS 目标 (可选，ON 时自动命名为 project::name)
#
# 用法示例:
#   rendu_add_library(
#       DIR          ${CMAKE_CURRENT_SOURCE_DIR}
#       PROJECT      ${PROJECT_NAME}
#       NAME         mylib
#       PRIVATE_LINK rendu-core-interface
#       PUBLIC_LINK  rendu::core
#       DEFINES      MYLIB_EXPORTS
#       STATIC
#       ALIAS        ON
#   )
# ====================================================================
function(rendu_add_library)
    # 参数解析
    set(options STATIC SHARED INTERFACE ALIAS)
    set(oneValueArgs DIR PROJECT NAME)
    set(multiValueArgs PRIVATE_LINK PUBLIC_LINK DEFINES SOURCES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 检查必需参数
    if (NOT ARG_NAME)
        rendu_log_fatal("rendu_add_library: 必须指定 NAME")
    endif ()
    if (NOT ARG_DIR)
        rendu_log_fatal("rendu_add_library: 必须指定 DIR")
    endif ()
    if (NOT IS_DIRECTORY "${ARG_DIR}")
        rendu_log_fatal("rendu_add_library: 源码目录不存在: ${ARG_DIR}")
    endif ()

    # 收集源文件
    if (NOT ARG_SOURCES)
        rendu_collect_source_files(
                src_list "${ARG_DIR}"
                EXTENSIONS .c .cc .cpp .inl
                EXCLUDE_DIRS
                "${ARG_DIR}/precompiled_headers"
        )
    else ()
        set(src_list ${ARG_SOURCES})
    endif ()

    # 生成目标名称
    set(target_name "${ARG_PROJECT}_${ARG_NAME}")

    # 创建库，自动判断是否为 header-only
    if (src_list)
        if (ARG_STATIC)
            set(lib_type STATIC)
        elseif (ARG_SHARED)
            set(lib_type SHARED)
        elseif (ARG_INTERFACE)
            set(lib_type INTERFACE)
        else ()
            set(lib_type STATIC)  # 默认静态库
        endif ()

        add_library(${target_name} ${lib_type} ${src_list})
        rendu_log_debug("${target_name} ${lib_type} 库")
    else ()
        set(lib_type INTERFACE)
        add_library(${target_name} ${lib_type})
        rendu_log_debug("${target_name} 自动转为 header-only ${lib_type} 库 (无源文件)")
    endif ()

    # 设置目标属性
    if (ARG_PROJECT)
        set_target_properties(${target_name} PROPERTIES PROJECT_LABEL "${ARG_PROJECT}")
        set_target_properties(${target_name} PROPERTIES FOLDER "${ARG_PROJECT}/${ARG_NAME}")
    endif ()

    # 自动收集 include 目录
    rendu_collect_include_directories(include_dirs "${ARG_DIR}"
            EXCLUDE_DIRS
            "${ARG_DIR}/tests"
            "${CMAKE_BINARY_DIR}"
            EXCLUDE_REGEX ".*/private"
    )

    # 根据库类型确定包含目录可见性
    if (lib_type STREQUAL "INTERFACE")
        set(visibility INTERFACE)
    else ()
        set(visibility PUBLIC)
    endif ()

    # 添加包含目录 (使用 target_include_directories 而非全局 include_directories)
    if (DEFINED RENDU_BUILDDIR)
        target_include_directories(${target_name}
                ${visibility}
                ${RENDU_BUILDDIR}
        )
    endif ()
    target_include_directories(${target_name}
            ${visibility}
            ${include_dirs}
    )

    # 链接接口库
    if (ARG_PRIVATE_LINK)
        if (lib_type STREQUAL "INTERFACE")
            target_link_libraries(${target_name} INTERFACE ${ARG_PRIVATE_LINK})
        else ()
            target_link_libraries(${target_name} PRIVATE ${ARG_PRIVATE_LINK})
        endif ()
    endif ()

    # 链接依赖
    if (ARG_PUBLIC_LINK)
        if (lib_type STREQUAL "INTERFACE")
            target_link_libraries(${target_name} INTERFACE ${ARG_PUBLIC_LINK})
        else ()
            target_link_libraries(${target_name} PUBLIC ${ARG_PUBLIC_LINK})
        endif ()
    endif ()

    # 添加预处理宏
    if (ARG_DEFINES)
        if (lib_type STREQUAL "INTERFACE")
            target_compile_definitions(${target_name} INTERFACE ${ARG_DEFINES})
        else ()
            target_compile_definitions(${target_name} PRIVATE ${ARG_DEFINES})
        endif ()
    endif ()

    # 创建 ALIAS 目标，命名规范为 project::name
    if (ARG_ALIAS AND ARG_PROJECT)
        set(alias_name "${ARG_PROJECT}::${ARG_NAME}")
        rendu_log_debug("${alias_name} 作为 ALIAS 目标")
        add_library(${alias_name} ALIAS ${target_name})
    endif ()

    rendu_log_info("添加库目标 ${target_name}")
endfunction()
