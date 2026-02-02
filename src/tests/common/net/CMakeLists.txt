#**********************************
#  Created by boil on 2026/1/24.
#**********************************

# Codec 测试
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME codec_test
        SOURCES codec_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)

# Socket 测试
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME socket_test
        SOURCES socket_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)

# Channel 测试
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME channel_test
        SOURCES channel_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)

# ConnectionPool 测试
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME connection_pool_test
        SOURCES connection_pool_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)

# BufferView 测试
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME buffer_view_test
        SOURCES buffer_view_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)

# UDP 测试
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME udp_test
        SOURCES udp_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)

# TCP 优化测试
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME tcp_optimization_test
        SOURCES tcp_optimization_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common
        TEST_FRAMEWORK CATCH2
)

# KCP 测试（占位，需要集成 KCP 库）
# rendu_add_test(
#         DIR           ${CMAKE_CURRENT_SOURCE_DIR}
#         PROJECT       ${PROJECT_NAME}
#         NAME          kcp_test
#         SOURCES       kcp_test.cpp
#         PRIVATE_LINK  rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
#         PUBLIC_LINK   RenduCore::common
#         TEST_FRAMEWORK CATCH2
# )


rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME remote_channel_test
        SOURCES remote_channel_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::common protobuf::libprotobuf remote_message_proto
        TEST_FRAMEWORK CATCH2
)


