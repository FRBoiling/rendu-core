#**********************************
#  Created by boil on 2026/1/21.
#**********************************

rendu_add_test(
    DIR           ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT       ${PROJECT_NAME}
    NAME          string_test
    SOURCES       string_test.cpp
    PRIVATE_LINK  rendu-core-interface
    PUBLIC_LINK   Catch2::Catch2WithMain RenduCore::common
    TEST_FRAMEWORK CATCH2
)

rendu_add_test(
    DIR           ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT       ${PROJECT_NAME}
    NAME          time_test
    SOURCES       time_test.cpp
    PRIVATE_LINK  rendu-core-interface
    PUBLIC_LINK   Catch2::Catch2WithMain RenduCore::common
    TEST_FRAMEWORK CATCH2
)

rendu_add_test(
    DIR           ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT       ${PROJECT_NAME}
    NAME          error_test
    SOURCES       error_test.cpp
    PRIVATE_LINK  rendu-core-interface
    PUBLIC_LINK   Catch2::Catch2WithMain RenduCore::common
    TEST_FRAMEWORK CATCH2
)

rendu_add_test(
    DIR           ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT       ${PROJECT_NAME}
    NAME          container_test
    SOURCES       container_test.cpp
    PRIVATE_LINK  rendu-core-interface
    PUBLIC_LINK   Catch2::Catch2WithMain RenduCore::common
    TEST_FRAMEWORK CATCH2
)
