#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(protobuf_source_dir ${CMAKE_CURRENT_SOURCE_DIR})
# Find revision ID and hash of the sourcetree
include(cmake/libprotobuf.cmake)
# set default buildoptions and print them
include(cmake/libprotobuf-lite.cmake)
# print out the results before continuing
include(cmake/libprotoc.cmake)

rendu_add_library(
    NAME
    protobuf
    SRC_DIR
    ${protobuf_source_dir}
    SRCS
    ${libprotobuf_lite_files}
    ${libprotobuf_files}
    HDRS
    ${libprotobuf_includes}
    PRIVATE
    rendu-dependency-interface
    DEFINES
    DEPS
    LINKOPTS
)