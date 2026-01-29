# 客户端应用 CMakeLists.txt
# 阶段14：应用层 - 客户端测试工具
# 基于 RenduCore 框架实现的客户端测试工具

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "正在配置 ${FOLDER_NAME} 客户端应用")

# 测试客户端库
rendu_add_library(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    test_client_lib
    PUBLIC_LINK
    RenduCore::core
    protobuf::libprotobuf
    Boost::system
    fmt::fmt
    SOURCES
    test_client.cpp
    test_client.h
    performance_test_config.h
    performance_test.h
    performance_test.cpp
    simple_test.h
    simple_test.cpp
    ${PROTO_SRCS}
)

target_include_directories(${PROJECT_NAME}_test_client_lib PUBLIC
    ${PROTO_GEN_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 客户端可执行文件 (支持 test 和 perf 两种模式)
rendu_add_executable(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    client
    PRIVATE_LINK
    rendu-core-interface
    PUBLIC_LINK
    ${PROJECT_NAME}_test_client_lib
    SOURCES
    main.cpp
)

target_include_directories(${PROJECT_NAME}_client PRIVATE
    ${PROTO_GEN_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 依赖 protobuf 生成
add_dependencies(${PROJECT_NAME}_client generate_protobuf)

# 设置输出目录
set_target_properties(${PROJECT_NAME}_client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
