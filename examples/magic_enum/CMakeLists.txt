#|--------------------------------
#| RD Application Project
#|--------------------------------

#|--------------------------------
#| RD Application Project
#|--------------------------------

set(target-name magic_enum)
set(header_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(sources_dir ${CMAKE_CURRENT_SOURCE_DIR})
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
    magic_enum::magic_enum
    PUBLIC
    )
