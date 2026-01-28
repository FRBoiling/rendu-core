#**********************************
#  Created by boil on 2026/1/26.
#**********************************

# ECS 测试
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME registry_test
        SOURCES registry_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)

rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME system_test
        SOURCES system_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)

rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME view_test
        SOURCES view_test.cpp
        PRIVATE_LINK  rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK   RenduCore::common
        TEST_FRAMEWORK CATCH2
)
