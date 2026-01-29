# 服务器应用 CMakeLists.txt
# 阶段14：应用层 - 服务器
# 基于 RenduCore 框架实现的完整服务器示例

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "正在配置 ${FOLDER_NAME} 服务器应用")

# 查找依赖
find_package(Boost REQUIRED COMPONENTS system)
find_package(Protobuf REQUIRED)

# 设置 Protobuf 生成目录
set(PROTO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/protocol)
set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto_gen)
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

set(PROTO_FILES
    ${PROTO_SRC_DIR}/messages.proto
)

# 生成 Protobuf 源文件和头文件
set(PROTO_SRCS)
set(PROTO_HDRS)

foreach(proto_file ${PROTO_FILES})
    get_filename_component(proto_name ${proto_file} NAME_WE)

    set(proto_src "${PROTO_GEN_DIR}/${proto_name}.pb.cc")
    set(proto_hdr "${PROTO_GEN_DIR}/${proto_name}.pb.h")

    list(APPEND PROTO_SRCS ${proto_src})
    list(APPEND PROTO_HDRS ${proto_hdr})

    add_custom_command(
        OUTPUT ${proto_src} ${proto_hdr}
        COMMAND protobuf::protoc
        ARGS --proto_path=${PROTO_SRC_DIR}
             --cpp_out=${PROTO_GEN_DIR}
             ${proto_file}
        DEPENDS ${proto_file}
        COMMENT "Generating protobuf code for ${proto_name}"
    )
endforeach()

# 服务器可执行文件
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
        protobuf::libprotobuf
        Boost::system
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
add_custom_target(proto_generate DEPENDS ${PROTO_SRCS} ${PROTO_HDRS})
add_dependencies(${PROJECT_NAME}_server proto_generate)

# 设置输出目录
set_target_properties(${PROJECT_NAME}_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 添加 protobuf 生成目录到源文件属性
set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES
    GENERATED TRUE
    SKIP_PRECOMPILE_HEADERS ON
)
