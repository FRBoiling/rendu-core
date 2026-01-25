#**********************************
#  Created by boil on 2026/1/25.
#**********************************

# 生成 protobuf 文件
find_package(Protobuf REQUIRED)
set(PROTO_FILES test_messages.proto)

# 生成 .pb.cc 和 .pb.h
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# Protobuf 测试
rendu_add_test(
        DIR           ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT       ${PROJECT_NAME}
        NAME          protobuf_ser_test
        SOURCES       protobuf_ser_test.cpp ${PROTO_SRCS} ${PROTO_HDRS}
        PRIVATE_LINK  rendu-core-interface
        PUBLIC_LINK   3rdparty::catch RenduCore::common
        TEST_FRAMEWORK CATCH2
)

# 包含 protobuf 生成的头文件目录
target_include_directories(${PROJECT_NAME}_protobuf_ser_test PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# JSON 测试
rendu_add_test(
        DIR           ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT       ${PROJECT_NAME}
        NAME          json_ser_test
        SOURCES       json_ser_test.cpp
        PRIVATE_LINK  rendu-core-interface
        PUBLIC_LINK   3rdparty::catch RenduCore::common
        TEST_FRAMEWORK CATCH2
)
