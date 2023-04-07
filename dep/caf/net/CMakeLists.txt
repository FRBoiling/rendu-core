rendu_add_component(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    caf_net
    HDRS
#    ${caf_net_headers}
    SRCS
#    ${caf_net_files}
    LINKOPTS
    rendu-dependency-interface
    DEPS
    dep::caf_core
    dep::openssl
)

