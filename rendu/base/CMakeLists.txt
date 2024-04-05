#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(target_name base)
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
    third_party::entt
    third_party::spdlog
    third_party::date
    third_party::fmt
)
