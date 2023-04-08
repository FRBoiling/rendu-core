
rendu_add_library(
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
    LINKOPTS
    rendu-dependency-interface
    DEPS
    dep::caf_core
)

