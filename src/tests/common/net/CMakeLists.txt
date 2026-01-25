#**********************************
#  Created by boil on 2026/1/24.
#**********************************

# Net 测试主入口（提供 main 函数）
rendu_add_test(
        DIR           ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT       ${PROJECT_NAME}
        NAME          all_net_tests
        SOURCES       all_net_tests.cpp
        PRIVATE_LINK  rendu-core-interface RenduCore::common
        PUBLIC_LINK   3rdparty::catch
        TEST_FRAMEWORK CATCH2
)

# Codec 测试
rendu_add_test(
        DIR           ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT       ${PROJECT_NAME}
        NAME          codec_test
        SOURCES       codec_test.cpp
        PRIVATE_LINK  rendu-core-interface
        PUBLIC_LINK   3rdparty::catch RenduCore::common
        TEST_FRAMEWORK CATCH2
)

# Socket 测试
rendu_add_test(
        DIR           ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT       ${PROJECT_NAME}
        NAME          socket_test
        SOURCES       socket_test.cpp
        PRIVATE_LINK  rendu-core-interface
        PUBLIC_LINK   3rdparty::catch RenduCore::common
        TEST_FRAMEWORK CATCH2
)

# Channel 测试
rendu_add_test(
        DIR           ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT       ${PROJECT_NAME}
        NAME          channel_test
        SOURCES       channel_test.cpp
        PRIVATE_LINK  rendu-core-interface
        PUBLIC_LINK   3rdparty::catch RenduCore::common 3rdparty::boost
        TEST_FRAMEWORK CATCH2
)
