#**********************************
#  Created by boil on 2026/1/22.
#**********************************

# 创建测试目标
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME logger_test
        SOURCES logger_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)

rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME formatter_test
        SOURCES formatter_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)

rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME console_sink_test
        SOURCES console_sink_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)

rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME file_sink_test
        SOURCES file_sink_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)

# AsyncBufferedLogger 测试
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME async_buffered_logger_test
        SOURCES async_buffered_logger_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)
