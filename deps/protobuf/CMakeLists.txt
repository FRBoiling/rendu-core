#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(target-name protobuf)
set(project-name deps)
set(header_dir ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(sources_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
list(APPEND RENDU_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

message("")
message(STATUS "Building " ${target-name} "...")

CollectSourceFiles(
    ${sources_dir}
    ${target-name}_sources
    # Exclude
)

CollectHeaderFiles(
    ${header_dir}
    ${target-name}_headers
    # Exclude
)

rendu_cc_library(${target-name} ${project-name}
    NAME
    ${target-name}
    HDRS
    ${${target-name}_headers}
    SRCS
    ${${target-name}_sources}
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    PUBLIC
    )
