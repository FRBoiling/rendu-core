# 服务器应用 CMakeLists.txt
# 阶段14：应用层 - 服务器
# 基于 RenduCore 框架实现的完整服务器示例

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "正在配置 ${FOLDER_NAME} 服务器应用")

# 可执行文件
rendu_add_executable(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    server
    PRIVATE_LINK
    rendu-core-interface
    PUBLIC_LINK
    RenduCore::core
    DEFINES
    ALIAS
    ON
    SOURCES
    main.cpp
    server_actor.cpp
    session_actor.cpp
    ${PROTO_SRCS}
)

# 添加到头文件搜索路径
target_include_directories(${PROJECT_NAME}_server PRIVATE
    ${PROTO_GEN_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 依赖 protobuf 生成
add_dependencies(${PROJECT_NAME}_server generate_protobuf)

# 设置输出目录
set_target_properties(${PROJECT_NAME}_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
