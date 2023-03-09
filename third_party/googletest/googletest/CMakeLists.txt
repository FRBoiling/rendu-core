set(target_name gtest)
rendu_add_library(
    NAME
    ${target_name}
    SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
    rendu-dependency-interface
    DEPS
)