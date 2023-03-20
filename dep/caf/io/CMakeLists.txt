
rendu_add_component(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    caf_io
    HDRS
#    ${caf_io_headers}
    SRCS
#    ${caf_io_files}
    SETTING
    rendu-dependency-interface
    DEPS
    dep::caf_core
    LINKOPTS
)

