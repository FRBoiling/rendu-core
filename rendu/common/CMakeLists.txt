#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(target_name common)
rendu_add_library(
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    BIN_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
    PRIVATE
    rendu-core-interface
    DEPS
)
