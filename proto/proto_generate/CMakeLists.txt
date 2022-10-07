#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(EXEC_NAME ${PROTOBUF_PROTOC_EXECUTABLE})

CollectSourceFiles(
    ${CMAKE_CURRENT_SOURCE_DIR}
    private_sources
)

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROTOBUF_PROTOC_EXECUTABLE}
    ${private_sources}
    )

target_link_libraries(${PROTOBUF_PROTOC_EXECUTABLE}
    PRIVATE
    rendu-core-interface
    PUBLIC
    libprotoc libprotobuf
    #TODO:BOIL 添加依赖项
    )

CollectIncludeDirectories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    public_includes
)

target_include_directories(${PROTOBUF_PROTOC_EXECUTABLE}
    PUBLIC
    ${public_includes}
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

