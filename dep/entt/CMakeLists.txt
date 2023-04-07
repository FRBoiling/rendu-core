#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(target_name entt)
rendu_add_component(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    LINKOPTS
    rendu-dependency-interface
    DEPS
)

