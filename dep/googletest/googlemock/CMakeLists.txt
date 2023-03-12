set(target_name gmock)
rendu_add_library(
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    SETTING
    rendu-dependency-interface
    DEPS
    dep::gtest
)