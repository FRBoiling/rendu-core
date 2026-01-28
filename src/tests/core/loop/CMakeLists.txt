#**********************************
#  Created by boil on 2026/1/26.
#**********************************

# FixedLoop 测试
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME fixed_loop_test
        SOURCES fixed_loop_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::core
        TEST_FRAMEWORK CATCH2
)
