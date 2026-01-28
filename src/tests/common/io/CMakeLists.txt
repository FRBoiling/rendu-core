#**********************************
#  Created by boil on 2026/1/22.
#**********************************

# 创建测试目标
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME io_context_test
        SOURCES io_context_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)

rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME scheduler_test
        SOURCES scheduler_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)

rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME timer_test
        SOURCES timer_test.cpp
        PRIVATE_LINK  rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK   RenduCore::common
        TEST_FRAMEWORK CATCH2
)
