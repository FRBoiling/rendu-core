set(openssl_dir ${CMAKE_CURRENT_SOURCE_DIR}/openssl)

CollectHeaderFiles(
    ${openssl_dir}/openssl
    caf_openssl_headers
)

set(caf_openssl_files
    ${openssl_dir}/src/openssl/manager.cpp
    ${openssl_dir}/src/openssl/middleman_actor.cpp
    ${openssl_dir}/src/openssl/publish.cpp
    ${openssl_dir}/src/openssl/remote_actor.cpp
    ${openssl_dir}/src/openssl/session.cpp
    )

rendu_add_library(
    DIR
    ${openssl_dir}
    PROJECT
    ${PROJECT_NAME}
    NAME
    caf_openssl
    HDRS
    ${caf_openssl_headers}
    SRCS
    ${caf_openssl_files}
    SETTING
    rendu-dependency-interface
    DEPS
    dep::caf_io
    dep::openssl
    LINKOPTS
)

