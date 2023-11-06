#**********************************
#  Created by boil on 2023/4/9.
#**********************************

set(target_name rendu_net)
rendu_add_library(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    LINKOPTS
    rendu-core-interface
    DEPS
    dep::rendu_common
)
