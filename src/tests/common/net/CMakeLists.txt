#**********************************
#  Created by boil on 2026/1/24.
#**********************************

# Net 测试主入口（提供 main 函数）- 已禁用，因为这是空的聚合器，其他测试已经独立运行
# rendu_add_test(
#         DIR           ${CMAKE_CURRENT_SOURCE_DIR}
#         PROJECT       ${PROJECT_NAME}
#         NAME          all_net_tests
#         SOURCES       all_net_tests.cpp
#         PRIVATE_LINK  rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
#         PUBLIC_LINK   RenduCore::common
#         TEST_FRAMEWORK CATCH2
# )

# Codec 测试
rendu_add_test(
        DIR           ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT       ${PROJECT_NAME}
        NAME          codec_test
        SOURCES       codec_test.cpp
        PRIVATE_LINK  rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK   RenduCore::common
        TEST_FRAMEWORK CATCH2
)

# Socket 测试
rendu_add_test(
        DIR           ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT       ${PROJECT_NAME}
        NAME          socket_test
        SOURCES       socket_test.cpp
        PRIVATE_LINK  rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK   RenduCore::common
        TEST_FRAMEWORK CATCH2
)

# Channel 测试
rendu_add_test(
        DIR           ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT       ${PROJECT_NAME}
        NAME          channel_test
        SOURCES       channel_test.cpp
        PRIVATE_LINK  rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK   RenduCore::common
        TEST_FRAMEWORK CATCH2
)

# UDP 测试
rendu_add_test(
        DIR           ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT       ${PROJECT_NAME}
        NAME          udp_test
        SOURCES       udp_test.cpp
        PRIVATE_LINK  rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK   RenduCore::common
        TEST_FRAMEWORK CATCH2
)

# KCP 测试（占位，需要集成 KCP 库）
# rendu_add_test(
#         DIR           ${CMAKE_CURRENT_SOURCE_DIR}
#         PROJECT       ${PROJECT_NAME}
#         NAME          kcp_test
#         SOURCES       kcp_test.cpp
#         PRIVATE_LINK  rendu-core-interface
#         PUBLIC_LINK   3rdparty::catch RenduCore::common 3rdparty::boost
#         TEST_FRAMEWORK CATCH2
# )
