rendu_add_component(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    caf_ping_pong_define
    HDRS
#    ${caf_openssl_headers}
    SRCS
#    ${caf_openssl_files}
    SETTING
    rendu-dependency-interface
    DEPS
    dep::caf_io
    dep::caf_core
    dep::protobuf
    proto::model
    LINKOPTS
)

