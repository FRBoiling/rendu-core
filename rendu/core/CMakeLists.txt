#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(target_name core)
rendu_add_library(
    NAME
    ${target_name}
    SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
    rendu-core-interface
    DEPS
    rendu::common
)
