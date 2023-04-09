#**********************************
#  Created by boil on 2022/10/24.
#**********************************
set(target_name ecs)
rendu_add_executable(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT
        ${PROJECT_NAME}
        NAME
        ${target_name}
        LINKOPTS
        rendu-core-interface
        DEPS
        proto::model
        dep::entt
        DEFINES
)

