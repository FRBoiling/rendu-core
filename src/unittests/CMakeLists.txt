#**********************************
# 单元测试
#**********************************

# =============================================
# 使用 rendu_add_test 添加独立的 ECS 单元测试
# 每个测试文件对应一个独立的测试可执行文件
# =============================================
if (RENDU_BUILD_TESTING)
    include(CTest)
    enable_testing()

    # 设置 Catch2 编译选项
    set(CATCH_CONFIG_MAIN true)

    # =============================================
    # 核心功能测试
    # =============================================

    # Registry 测试
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            registry_test
            SOURCES         registry_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core 3rdparty::catch
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit core registry
    )

    # Archetype 测试
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            archetype_test
            SOURCES         archetype_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core 3rdparty::catch
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit core archetype
    )

    # Signal 测试
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            signal_test
            SOURCES         signal_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core 3rdparty::catch
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit core signal
    )

    # Resource 测试
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            resource_test
            SOURCES         resource_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core 3rdparty::catch
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit core resource
    )

    # Migration 测试
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            migration_test
            SOURCES         migration_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core 3rdparty::catch
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit core migration
    )

    # =============================================
    # 高级功能测试
    # =============================================

    # Relations 测试
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            relations_test
            SOURCES         relations_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core 3rdparty::catch
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit advanced relations
    )

    # Tools 测试
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            tools_test
            SOURCES         tools_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core 3rdparty::catch
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit advanced tools
    )

    # Debugging 测试
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            debugging_test
            SOURCES         debugging_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core 3rdparty::catch
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit advanced debugging
    )

    # Logger 测试
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            logger_test
            SOURCES         logger_test.cpp
            PRIVATE_LINK    rendu-core-interface RenduCore::common
            PUBLIC_LINK     3rdparty::fmt 3rdparty::catch
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit logger
    )

    # Network 测试
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            network_test
            SOURCES         network_test.cpp
            PRIVATE_LINK    rendu-core-interface RenduCore::common
            PUBLIC_LINK     3rdparty::catch
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit network
    )

    # Performance 测试
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            performance_test
            SOURCES         performance_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core 3rdparty::catch
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit advanced performance
    )

    # =============================================
    # 快速测试
    # =============================================

    # Benchmark 测试
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            benchmark_test
            SOURCES         benchmark_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core 3rdparty::catch
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit fast benchmark
    )

    # =============================================
    # 可选测试（运行时间较长）
    # =============================================

    # Profiling 测试
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            profiling_test
            SOURCES         profiling_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core 3rdparty::catch
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         600
            LABELS          unit optional profiling
    )

    # =============================================
    # 测试分组和快捷命令
    # =============================================

    # 核心功能测试套件
    rendu_add_custom_target(
        PROJECT         ${PROJECT_NAME}
        NAME            core_tests
        COMMAND         ${CMAKE_CTEST_COMMAND}
        COMMAND_ARGS     --output-on-failure -L core
        DEPENDS         ${PROJECT_NAME}_registry_test
                        ${PROJECT_NAME}_archetype_test
                        ${PROJECT_NAME}_signal_test
                        ${PROJECT_NAME}_resource_test
                        ${PROJECT_NAME}_migration_test
        COMMENT         "运行核心功能测试..."
        WORKING_DIR     ${CMAKE_BINARY_DIR}
    )

    # 高级功能测试套件
    rendu_add_custom_target(
        PROJECT         ${PROJECT_NAME}
        NAME            advanced_tests
        COMMAND         ${CMAKE_CTEST_COMMAND}
        COMMAND_ARGS     --output-on-failure -L advanced
        DEPENDS         ${PROJECT_NAME}_relations_test
                        ${PROJECT_NAME}_tools_test
                        ${PROJECT_NAME}_debugging_test
                        ${PROJECT_NAME}_logger_test
                        ${PROJECT_NAME}_network_test
                        ${PROJECT_NAME}_performance_test
        COMMENT         "运行高级功能测试..."
        WORKING_DIR     ${CMAKE_BINARY_DIR}
    )

    # 快速测试套件（CI/CD 使用）
    rendu_add_custom_target(
        PROJECT         ${PROJECT_NAME}
        NAME            fast_tests
        COMMAND         ${CMAKE_CTEST_COMMAND}
        COMMAND_ARGS     --output-on-failure -L fast
        DEPENDS         ${PROJECT_NAME}_benchmark_test
        COMMENT         "运行快速基准测试..."
        WORKING_DIR     ${CMAKE_BINARY_DIR}
    )

    # 所有测试（不包括可选的 profiling）
    rendu_add_custom_target(
        PROJECT         ${PROJECT_NAME}
        NAME            run_unit_tests
        COMMAND         ${CMAKE_CTEST_COMMAND}
        COMMAND_ARGS     --output-on-failure -L unit -E optional
        DEPENDS         ${PROJECT_NAME}_registry_test
                        ${PROJECT_NAME}_archetype_test
                        ${PROJECT_NAME}_signal_test
                        ${PROJECT_NAME}_resource_test
                        ${PROJECT_NAME}_migration_test
                        ${PROJECT_NAME}_relations_test
                        ${PROJECT_NAME}_tools_test
                        ${PROJECT_NAME}_debugging_test
                        ${PROJECT_NAME}_logger_test
                        ${PROJECT_NAME}_network_test
                        ${PROJECT_NAME}_performance_test
                        ${PROJECT_NAME}_benchmark_test
        COMMENT         "运行所有单元测试..."
        WORKING_DIR     ${CMAKE_BINARY_DIR}
    )

    # 完整测试套件（包括所有测试）
    rendu_add_custom_target(
        PROJECT         ${PROJECT_NAME}
        NAME            run_all_tests_full
        COMMAND         ${CMAKE_CTEST_COMMAND}
        COMMAND_ARGS     --output-on-failure -L unit
        DEPENDS         ${PROJECT_NAME}_registry_test
                        ${PROJECT_NAME}_archetype_test
                        ${PROJECT_NAME}_signal_test
                        ${PROJECT_NAME}_resource_test
                        ${PROJECT_NAME}_migration_test
                        ${PROJECT_NAME}_relations_test
                        ${PROJECT_NAME}_tools_test
                        ${PROJECT_NAME}_debugging_test
                        ${PROJECT_NAME}_logger_test
                        ${PROJECT_NAME}_network_test
                        ${PROJECT_NAME}_performance_test
                        ${PROJECT_NAME}_benchmark_test
                        ${PROJECT_NAME}_profiling_test
        COMMENT         "运行完整测试套件（包括性能分析）..."
        WORKING_DIR     ${CMAKE_BINARY_DIR}
    )

endif()
