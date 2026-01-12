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

    # =============================================
    # Archetype 测试
    # =============================================
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            archetype_test
            SOURCES         archetype_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core Catch2::Catch2WithMain
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit archetype core
    )

    # =============================================
    # Registry 测试
    # =============================================
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            registry_test
            SOURCES         registry_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core Catch2::Catch2WithMain
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit registry core
    )

    # =============================================
    # Relations 测试
    # =============================================
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            relations_test
            SOURCES         relations_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core Catch2::Catch2WithMain
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit relations core
    )

    # =============================================
    # Signal 测试
    # =============================================
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            signal_test
            SOURCES         signal_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core Catch2::Catch2WithMain
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit signal core
    )

    # =============================================
    # Migration 测试
    # =============================================
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            migration_test
            SOURCES         migration_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core Catch2::Catch2WithMain
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit migration core
    )

    # =============================================
    # Performance 测试
    # =============================================
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            performance_test
            SOURCES         performance_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core Catch2::Catch2WithMain
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit performance core
    )

    # =============================================
    # Tools 测试
    # =============================================
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            tools_test
            SOURCES         tools_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core Catch2::Catch2WithMain
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit tools core
    )

    # =============================================
    # Debugging 测试
    # =============================================
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            debugging_test
            SOURCES         debugging_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core Catch2::Catch2WithMain
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit debugging core
    )

    # =============================================
    # Resource 测试
    # =============================================
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            resource_test
            SOURCES         resource_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core Catch2::Catch2WithMain
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit resource core
    )

    # =============================================
    # Benchmark 测试
    # =============================================
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            benchmark_test
            SOURCES         benchmark_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core Catch2::Catch2WithMain
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit benchmark core
    )

    # =============================================
    # 性能分析测试（可选）
    # =============================================
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            profiling_test
            SOURCES         profiling_test.cpp
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core Catch2::Catch2WithMain
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         600
            LABELS          unit profiling core
    )

    # =============================================
    # 运行所有测试的自定义目标
    # =============================================
    rendu_add_test(
            DIR             ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT         ${PROJECT_NAME}
            NAME            run_all_tests
            PRIVATE_LINK    rendu-core-interface
            PUBLIC_LINK     RenduCore::core Catch2::Catch2WithMain
            DEFINES         CATCH_CONFIG_ENABLE_BENCHMARKING
            TEST_FRAMEWORK  CATCH2
            TIMEOUT         300
            LABELS          unit all core
    )

endif()
