# ====================================================================
# 模块: RenduAddTest
# 描述: 增强版测试目标创建
# 依赖模块:
#   - RenduLogging (日志)
#   - RenduCollectDirectories (目录收集)
#   - RenduCollectFiles (文件收集)
#
# 建议通过 RenduCore.cmake 统一加载所有模块
# ====================================================================

# ====================================================================
# 函数: rendu_add_test
# 描述: 增强版 add_test，自动收集测试源文件、配置测试目标
#
# 参数:
#   DIR           - 源码目录 (必填)
#   PROJECT       - 项目名 (可选，用于 IDE 工程标签)
#   NAME          - 目标名 (必填)
#   PRIVATE_LINK  - 需要 PRIVATE 链接的接口库 (可选)
#   PUBLIC_LINK   - 需要 PUBLIC 链接的依赖库 (可选)
#   DEFINES       - 需要添加的预处理宏 (可选)
#   SOURCES       - 指定源文件列表 (可选，未指定则自动收集)
#   TEST_FRAMEWORK - 测试框架类型 (可选，CATCH2|GTEST|NONE，默认 NONE)
#   WORKING_DIR   - 测试工作目录 (可选，默认为构建目录)
#   LABELS        - 测试标签列表 (可选，用于 ctest 分类)
#   TIMEOUT       - 测试超时时间 (可选，单位秒，默认 300)
#   COMMAND_ARGS  - 传递给测试的命令行参数 (可选)
#   ALIAS         - 是否创建 ALIAS 目标 (可选，默认 ON)
#
# 用法示例:
#   # Catch2 测试
#   rendu_add_test(
#       DIR           ${CMAKE_CURRENT_SOURCE_DIR}
#       PROJECT       ${PROJECT_NAME}
#       NAME          my_test
#       PRIVATE_LINK  rendu-core-interface
#       PUBLIC_LINK   3rdparty::catch
#       TEST_FRAMEWORK CATCH2
#       LABELS        unit integration
#       TIMEOUT       120
#   )
#
#   # GoogleTest 测试
#   rendu_add_test(
#       DIR           ${CMAKE_CURRENT_SOURCE_DIR}
#       PROJECT       ${PROJECT_NAME}
#       NAME          gtest_example
#       PRIVATE_LINK  rendu-core-interface
#       PUBLIC_LINK   GTest::gtest_main
#       TEST_FRAMEWORK GTEST
#       LABELS        unit
#   )
#
#   # 自定义测试
#   rendu_add_test(
#       DIR           ${CMAKE_CURRENT_SOURCE_DIR}
#       NAME          custom_test
#       SOURCES       test.cpp main.cpp
#       COMMAND_ARGS  --verbose
#   )
# ====================================================================
function(rendu_add_test)
    # 参数解析
    set(options ALIAS)
    set(oneValueArgs DIR PROJECT NAME TEST_FRAMEWORK WORKING_DIR TIMEOUT)
    set(multiValueArgs PRIVATE_LINK PUBLIC_LINK DEFINES SOURCES LABELS COMMAND_ARGS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 检查必需参数
    if (NOT ARG_NAME)
        rendu_log_fatal("rendu_add_test: 必须指定 NAME")
    endif ()
    if (NOT ARG_DIR)
        rendu_log_fatal("rendu_add_test: 必须指定 DIR")
    endif ()
    if (NOT IS_DIRECTORY "${ARG_DIR}")
        rendu_log_fatal("rendu_add_test: 测试目录不存在: ${ARG_DIR}")
    endif ()

    # 设置默认值
    if (NOT ARG_TEST_FRAMEWORK)
        set(ARG_TEST_FRAMEWORK "NONE")
    endif ()

    if (NOT ARG_TIMEOUT)
        set(ARG_TIMEOUT 300)
    endif ()

    if (NOT ARG_WORKING_DIR)
        set(ARG_WORKING_DIR "${CMAKE_BINARY_DIR}")
    endif ()

    # 收集源文件
    if (NOT ARG_SOURCES)
        rendu_collect_source_files(src_list "${ARG_DIR}")
    else ()
        set(src_list ${ARG_SOURCES})
    endif ()

    # 检查是否有源文件
    if (NOT src_list)
        rendu_log_warn("rendu_add_test: 未找到测试源文件")
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
        set_target_properties(${target_name} PROPERTIES FOLDER "Tests/${ARG_PROJECT}")
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

    # 创建 ALIAS 目标
    if (ARG_ALIAS AND ARG_PROJECT)
        set(alias_name "${ARG_PROJECT}::${ARG_NAME}")
        add_executable(${alias_name} ALIAS ${target_name})
        rendu_log_debug("${alias_name} 作为 ALIAS 目标")
    endif ()

    # 配置测试框架特定设置
    if (ARG_TEST_FRAMEWORK STREQUAL "CATCH2")
        # Catch2 配置
        if (NOT ARG_DEFINES)
            target_compile_definitions(${target_name} PRIVATE
                CATCH_CONFIG_DISABLE_EXCEPTIONS
            )
        endif ()
    elseif (ARG_TEST_FRAMEWORK STREQUAL "GTEST")
        # GoogleTest 配置 (已提供 main 函数)
        rendu_log_debug("配置 GoogleTest 测试")
    endif ()

    # 启用测试
    enable_testing()

    # 构建 CTest 命令
    set(test_command "${target_name}")

    # 添加命令行参数
    if (ARG_COMMAND_ARGS)
        list(APPEND test_command ${ARG_COMMAND_ARGS})
    endif ()

    # 添加测试
    add_test(
        NAME ${target_name}
        COMMAND ${test_command}
        WORKING_DIRECTORY ${ARG_WORKING_DIR}
    )

    # 设置测试属性
    set_tests_properties(${target_name} PROPERTIES
        TIMEOUT ${ARG_TIMEOUT}
        LABELS "${ARG_LABELS}"
    )

    # 为 Catch2 添加输出选项
    if (ARG_TEST_FRAMEWORK STREQUAL "CATCH2" AND BUILD_TESTING)
        set_tests_properties(${target_name} PROPERTIES
            PASS_REGULAR_EXPRESSION "All tests passed|test cases passed"
        )
    endif ()

    rendu_log_info("添加测试 ${target_name}")
    if (ARG_LABELS)
        rendu_log_info("  标签: ${ARG_LABELS}")
    endif ()
endfunction()

# ====================================================================
# 函数: rendu_add_testsuite
# 描述: 批量添加一组测试，适用于同一目录下的多个测试文件
#
# 参数:
#   DIR            - 测试源文件目录 (必填)
#   PROJECT        - 项目名 (必填)
#   PRIVATE_LINK   - 需要 PRIVATE 链接的接口库 (可选)
#   PUBLIC_LINK    - 需要 PUBLIC 链接的依赖库 (可选)
#   TEST_FRAMEWORK - 测试框架类型 (可选)
#   PATTERN        - 源文件匹配模式 (可选，默认 *_test.cpp)
#   EXCLUDE        - 排除的文件模式列表 (可选)
#   COMMON_DEFINES - 所有测试共享的宏定义 (可选)
#   COMMON_LABELS  - 所有测试共享的标签 (可选)
# ====================================================================
function(rendu_add_testsuite)
    # 参数解析
    set(oneValueArgs DIR PROJECT TEST_FRAMEWORK PATTERN)
    set(multiValueArgs PRIVATE_LINK PUBLIC_LINK EXCLUDE COMMON_DEFINES COMMON_LABELS)
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 检查必需参数
    if (NOT ARG_PROJECT)
        rendu_log_fatal("rendu_add_testsuite: 必须指定 PROJECT")
    endif ()
    if (NOT ARG_DIR)
        rendu_log_fatal("rendu_add_testsuite: 必须指定 DIR")
    endif ()

    # 设置默认模式
    if (NOT ARG_PATTERN)
        set(ARG_PATTERN "*_test.cpp")
    endif ()

    # 查找匹配的测试文件
    file(GLOB_RECURSE test_files
        "${ARG_DIR}/${ARG_PATTERN}"
    )

    # 过滤排除的文件
    if (ARG_EXCLUDE)
        foreach (exclude_pattern IN LISTS ARG_EXCLUDE)
            file(GLOB_RECURSE exclude_files "${ARG_DIR}/${exclude_pattern}")
            list(REMOVE_ITEM test_files ${exclude_files})
        endforeach ()
    endif ()

    # 为每个测试文件添加测试
    set(test_count 0)
    foreach (test_file IN LISTS test_files)
        # 获取文件名 (不带路径和扩展名)
        get_filename_component(test_basename ${test_file} NAME_WE)

        # 生成测试名
        set(test_name "${test_basename}")

        # 添加测试
        rendu_add_test(
            DIR             ${ARG_DIR}
            PROJECT         ${ARG_PROJECT}
            NAME            ${test_name}
            SOURCES         ${test_file}
            PRIVATE_LINK    ${ARG_PRIVATE_LINK}
            PUBLIC_LINK     ${ARG_PUBLIC_LINK}
            TEST_FRAMEWORK  ${ARG_TEST_FRAMEWORK}
            DEFINES         ${ARG_COMMON_DEFINES}
            LABELS          ${ARG_COMMON_LABELS}
        )

        math(EXPR test_count "${test_count} + 1")
    endforeach()

    rendu_log_info("添加了 ${test_count} 个测试")
endfunction()

# ====================================================================
# 函数: rendu_add_catch2_test
# 描述: 简化版 Catch2 测试添加函数
#
# 参数:
#   DIR           - 源码目录 (可选)
#   PROJECT       - 项目名 (可选)
#   NAME          - 目标名 (可选)
#   TIMEOUT       - 超时时间 (可选)
#   PRIVATE_LINK  - 私有链接库 (可选)
#   PUBLIC_LINK   - 公共链接库 (可选)
#   DEFINES       - 宏定义 (可选)
#   LABELS        - 测试标签 (可选)
#   COMMAND_ARGS  - 命令参数 (可选)
# ====================================================================
function(rendu_add_catch2_test)
    cmake_parse_arguments(ARG "" "DIR;PROJECT;NAME;TIMEOUT" "PRIVATE_LINK;PUBLIC_LINK;DEFINES;LABELS;COMMAND_ARGS" ${ARGN})

    rendu_add_test(
        DIR           ${ARG_DIR}
        PROJECT       ${ARG_PROJECT}
        NAME          ${ARG_NAME}
        PRIVATE_LINK  ${ARG_PRIVATE_LINK}
        PUBLIC_LINK   ${ARG_PUBLIC_LINK}
        DEFINES       ${ARG_DEFINES}
        LABELS        ${ARG_LABELS}
        COMMAND_ARGS  ${ARG_COMMAND_ARGS}
        TIMEOUT       ${ARG_TIMEOUT}
        TEST_FRAMEWORK CATCH2
    )
endfunction()

# ====================================================================
# 函数: rendu_add_gtest
# 描述: 简化版 GoogleTest 测试添加函数
#
# 参数:
#   DIR           - 源码目录 (可选)
#   PROJECT       - 项目名 (可选)
#   NAME          - 目标名 (可选)
#   TIMEOUT       - 超时时间 (可选)
#   PRIVATE_LINK  - 私有链接库 (可选)
#   PUBLIC_LINK   - 公共链接库 (可选)
#   DEFINES       - 宏定义 (可选)
#   LABELS        - 测试标签 (可选)
# ====================================================================
function(rendu_add_gtest)
    cmake_parse_arguments(ARG "" "DIR;PROJECT;NAME;TIMEOUT" "PRIVATE_LINK;PUBLIC_LINK;DEFINES;LABELS" ${ARGN})

    rendu_add_test(
        DIR           ${ARG_DIR}
        PROJECT       ${ARG_PROJECT}
        NAME          ${ARG_NAME}
        PRIVATE_LINK  ${ARG_PRIVATE_LINK}
        PUBLIC_LINK   ${ARG_PUBLIC_LINK}
        DEFINES       ${ARG_DEFINES}
        LABELS        ${ARG_LABELS}
        TIMEOUT       ${ARG_TIMEOUT}
        TEST_FRAMEWORK GTEST
    )
endfunction()
