set(target_name model)
rendu_add_component(
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    SETTING
    rendu-core-interface
    DEPS
    dep::protobuf
)