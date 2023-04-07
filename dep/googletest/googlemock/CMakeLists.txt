set(target_name gmock)
rendu_add_library(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    LINKOPTS
    rendu-dependency-interface
    DEPS
    dep::gtest
)