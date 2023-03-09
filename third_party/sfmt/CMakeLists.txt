#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(target_name sfmt)
rendu_add_library(
    NAME
    ${target_name}
    SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    DEPS
    PRIVATE
    rendu-dependency-interface
)

