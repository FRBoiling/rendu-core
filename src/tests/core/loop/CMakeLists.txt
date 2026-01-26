#**********************************
#  Created by boil on 2026/1/26.
#**********************************

# FixedLoop 测试
rendu_add_test(
    DIR           ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT       ${PROJECT_NAME}
    NAME          loop_test
    SOURCES       fixed_loop_test.cpp
    PRIVATE_LINK  rendu-core-interface
    PUBLIC_LINK   3rdparty::catch RenduCore::core
    TEST_FRAMEWORK CATCH2
)
