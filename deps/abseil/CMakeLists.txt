#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(target-name absl)
set(project-name deps)
set(header_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(sources_dir ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND RENDU_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

message("")
message(STATUS "Building " ${target-name} "...")

CollectSourceFiles(
    ${sources_dir}
    ${target-name}_sources
    # Exclude
    ${sources_dir}/absl/random/internal/gaussian_distribution_gentables.cc
    ${sources_dir}/absl/hash/internal/print_hash_of.cc
)

CollectHeaderFiles(
    ${header_dir}
    ${target-name}_headers
    # Exclude

)

rendu_cc_library(${project-name} ${target-name}
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


