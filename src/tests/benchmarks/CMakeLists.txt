#**********************************
#  Rendu Core Performance Benchmarks
#  Created by boil on 2026/2/1.
#**********************************

# 性能基准测试套件 (使用 Catch2 的 BENCHMARK 功能)
rendu_add_test(
        DIR           ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT       ${PROJECT_NAME}
        NAME          benchmark_suite
        SOURCES       benchmark_suite.cpp
        PRIVATE_LINK  rendu-core-interface Catch2::Catch2WithMain
        PUBLIC_LINK   RenduCore::core
        TEST_FRAMEWORK CATCH2
)

# 便于 CI/文档使用: cmake --build build --target benchmarks
add_custom_target(benchmarks ALL DEPENDS benchmark_suite)
