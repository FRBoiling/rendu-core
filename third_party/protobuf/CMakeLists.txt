#**********************************
#  Created by boil on 2022/8/14.
#**********************************
set(PROJECT_NAME protobuf)
set(protobuf_source_dir ${CMAKE_CURRENT_SOURCE_DIR})
# Find revision ID and hash of the sourcetree
include(libprotobuf.cmake)
# set default buildoptions and print them
include(libprotobuf-lite.cmake)
# print out the results before continuing
include(libprotoc.cmake)

rendu_add_dependence(
    NAME
    libprotobuf
    PRIVATE_SOURCES
    ${libprotobuf_lite_files}
    ${libprotobuf_files}
    PUBLIC_HEADERS
    ${libprotobuf_includes}
    CMAKE_CUR_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    CMAKE_CUR_BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_BINARY_DIR
    ${CMAKE_BINARY_DIR}
    PRIVATE_DEPS
    rendu-dependency-interface
    PUBLIC_DEPS
    DEFINES
    LINKOPTS
)

rendu_add_dependence(
    NAME
    libprotobuf-lite
    PRIVATE_SOURCES
    ${libprotobuf_lite_files}
    PUBLIC_HEADERS
    ${libprotobuf_lite_includes}
    CMAKE_CUR_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    CMAKE_CUR_BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_BINARY_DIR
    ${CMAKE_BINARY_DIR}
    PRIVATE_DEPS
    rendu-dependency-interface
    PUBLIC_DEPS
    DEFINES
    LINKOPTS
)

rendu_add_dependence(
    NAME
    libprotoc
    PRIVATE_SOURCES
    ${libprotoc_files}
    PUBLIC_HEADERS
    ${libprotoc_headers}
    CMAKE_CUR_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    CMAKE_CUR_BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_BINARY_DIR
    ${CMAKE_BINARY_DIR}
    PRIVATE_DEPS
    rendu-dependency-interface
    PUBLIC_DEPS
    DEFINES
    LINKOPTS
)