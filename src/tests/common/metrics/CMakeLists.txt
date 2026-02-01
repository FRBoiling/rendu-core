#**********************************
#  Metrics 测试
#**********************************

rendu_add_test(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT
        ${PROJECT_NAME}
        NAME
        metrics_collector_test
        SOURCES
        metrics_collector_test.cpp
        PRIVATE_LINK
        rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK
        RenduCore::common
        TEST_FRAMEWORK
        CATCH2
)
