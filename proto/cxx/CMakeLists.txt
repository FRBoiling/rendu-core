set(target_name model)
rendu_add_library(
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    LINKOPTS
    rendu-core-interface
    DEPS
    dep::protobuf
)