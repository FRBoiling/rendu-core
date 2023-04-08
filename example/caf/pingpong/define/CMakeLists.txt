rendu_add_library(
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
    LINKOPTS
    rendu-dependency-interface
    DEPS
    dep::caf
    dep::protobuf
    proto::model
)

