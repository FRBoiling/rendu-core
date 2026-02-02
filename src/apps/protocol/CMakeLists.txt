# 协议定义 CMakeLists.txt
# 阶段14：应用层 - 共享协议
# 用于生成 server 和 client 共享的 Protobuf 代码

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "正在配置 ${FOLDER_NAME} 协议定义")

# 查找依赖
find_package(Protobuf REQUIRED)

# 设置 Protobuf 源文件
set(PROTO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto_gen)
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

set(PROTO_FILES
    ${PROTO_SRC_DIR}/messages.proto
    ${PROTO_SRC_DIR}/remote_message.proto
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

# 创建 protobuf 生成目标
add_custom_target(generate_protobuf ALL DEPENDS ${PROTO_SRCS} ${PROTO_HDRS})

# 添加 protobuf 生成目录到源文件属性
set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES
    GENERATED TRUE
    SKIP_PRECOMPILE_HEADERS ON
)

# 创建 protobuf 静态库
add_library(remote_message_proto STATIC ${PROTO_SRCS})
target_include_directories(remote_message_proto PUBLIC ${PROTO_GEN_DIR})
target_link_libraries(remote_message_proto PUBLIC protobuf::libprotobuf)

# 导出变量供其他项目使用(设置到 apps 级别作用域)
set(PROTO_SRCS ${PROTO_SRCS} PARENT_SCOPE)
set(PROTO_HDRS ${PROTO_HDRS} PARENT_SCOPE)
set(PROTO_GEN_DIR ${PROTO_GEN_DIR} PARENT_SCOPE)

# 设置到全局作用域供 server 和 client 使用
set(PROTO_SRCS ${PROTO_SRCS} CACHE INTERNAL "Protobuf source files")
set(PROTO_HDRS ${PROTO_HDRS} CACHE INTERNAL "Protobuf header files")
set(PROTO_GEN_DIR ${PROTO_GEN_DIR} CACHE INTERNAL "Protobuf generated directory")

message(STATUS "  - 协议文件: ${PROTO_FILES}")
message(STATUS "  - 生成目录: ${PROTO_GEN_DIR}")
