#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(target-name common)
set(project-name rendu)
set(header_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(sources_dir ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND RENDU_COMMON_INCLUDE_DIRS ${header_dir})

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

list(APPEND project_copts

    ${RENDU_DEFAULT_COPTS}
    )

rendu_cc_library(${project-name} ${target-name}
    NAME
    ${target-name}
    HDRS
    ${${target-name}_headers}
    SRCS
    ${${target-name}_sources}
    COPTS
    ${project_copts}
    DEPS
    deps_fmt
    DEFINES
    LINKOPTS
    DEPS
    )

