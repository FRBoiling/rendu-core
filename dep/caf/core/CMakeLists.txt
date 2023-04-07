
rendu_add_component(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    caf_core
    HDRS
#    ${caf_core_headers}
    SRCS
#    ${caf_core_files}
    LINKOPTS
    rendu-dependency-interface
    DEPS
    dep::threads
)

