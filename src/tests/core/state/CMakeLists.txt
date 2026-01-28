#**********************************
#  Created by boil on 2026/1/27.
#**********************************

# State 测试
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME state_machine_test
        SOURCES state_machine_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::core
        TEST_FRAMEWORK CATCH2
)
