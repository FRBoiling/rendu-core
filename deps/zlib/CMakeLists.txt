#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(target-name zlib)
set(header_dir ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(sources_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)

message(STATUS ">>" ${target-name})

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
rendu_cc_library(${target-name} ${target-name}
    NAME
    ${target-name}
    HDRS
    ${${target-name}_headers}
    SRCS
    ${${target-name}_sources}
    COPTS
    ${project_copts}
    DEPS
    DEFINES
    LINKOPTS
    DEPS
    )
message(STATUS  ${target-name}  " Make Success." )
