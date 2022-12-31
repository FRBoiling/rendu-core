#|--------------------------------
#| RD Application Project
#|--------------------------------

set(target-name main)
set(header_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(sources_dir ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND RENDU_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
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

rendu_cc_executable(${project-name} ${target-name}
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
